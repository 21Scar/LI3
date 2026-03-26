/*
 * programa-testes.c
 *
 * Skeleton test harness for the project.
 * Usage: ./programa-testes <dataset_dir> <commands_file> <expected_dir> [repeats]
 *
 * - Runs the main program once to produce outputs in `resultados/`.
 * - Compares each generated `commandN_output.txt` with expected files.
 * - Reports per-query-type correctness and first mismatch line.
 * - Measures overall execution time and peak memory (via getrusage).
 * - Optionally measures average time per query type by running the program
 *   with a temporary commands file that contains only commands of that type.
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

// -----------------------
// Path helpers (so the harness can be run from any cwd)
// -----------------------
static int get_dirname_from_argv0(const char *argv0, char *out_dir, size_t out_sz) {
    if (!argv0 || !out_dir || out_sz == 0) return -1;

    char resolved[4096];
    if (!realpath(argv0, resolved)) return -1;

    char *slash = strrchr(resolved, '/');
    if (!slash) return -1;
    *slash = '\0';

    if (strlen(resolved) + 1 > out_sz) return -1;
    strcpy(out_dir, resolved);
    return 0;
}

static int join_path(char *out, size_t out_sz, const char *dir, const char *file) {
    if (!out || !dir || !file) return -1;
    int n = snprintf(out, out_sz, "%s/%s", dir, file);
    return (n < 0 || (size_t)n >= out_sz) ? -1 : 0;
}

// Read all lines from a file into a dynamically allocated array of strings.
// Returns number of lines via out_count. Caller must free each string and the array.
char **read_lines(const char *path, int *out_count) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int cap = 256;
    int cnt = 0;
    char **arr = malloc(sizeof(char*) * cap);
    while ((read = getline(&line, &len, f)) != -1) {
        if (read > 0 && line[read-1] == '\n') line[--read] = '\0';
        if (cnt >= cap) { cap *= 2; arr = realloc(arr, sizeof(char*) * cap); }
        arr[cnt++] = strdup(line);
    }
    free(line);
    fclose(f);
    *out_count = cnt;
    return arr;
}

// Run programa-principal with given dataset and commands file.
// Returns 0 on success. Measures wall time in seconds (out_seconds) and
// peak memory in kilobytes via ru_maxrss (out_kb).
int run_program_and_measure(const char *program_dir, const char *program_path, const char *dataset_dir, const char *commands_file, double *out_seconds, long *out_kb) {
    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0) return -1;

    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        // child
        if (program_dir && chdir(program_dir) != 0) {
            perror("chdir");
            _exit(127);
        }
        execl(program_path, program_path, dataset_dir, commands_file, (char*)NULL);
        // exec failed
        perror("exec");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return -1;

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0) return -1;
    double secs = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    struct rusage ru;
    if (getrusage(RUSAGE_CHILDREN, &ru) == 0) {
        // ru_maxrss: on Linux it's kilobytes, on macOS it's bytes. We'll return kilobytes.
#if defined(__APPLE__)
        *out_kb = ru.ru_maxrss / 1024;
#else
        *out_kb = ru.ru_maxrss;
#endif
    } else {
        *out_kb = -1;
    }

    *out_seconds = secs;
    return 0;
}

// Compare two files line-by-line. Return 0 if identical, otherwise return 1 and
// set *mismatch_line to the 1-based line number of the first difference.
int compare_files_first_mismatch(const char *expected, const char *actual, int *mismatch_line) {
    FILE *fe = fopen(expected, "r");
    FILE *fa = fopen(actual, "r");
    if (!fe || !fa) {
        if (fe) fclose(fe);
        if (fa) fclose(fa);
        *mismatch_line = 1;
        return 1;
    }
    char *le = NULL, *la = NULL;
    size_t ne = 0, na = 0;
    ssize_t re, ra;
    int line = 0;
    int differing = 0;
    while (1) {
        re = getline(&le, &ne, fe);
        ra = getline(&la, &na, fa);
        if (re == -1 && ra == -1) break; // both EOF
        line++;
        if (re == -1 || ra == -1) { differing = 1; break; }
        // strip trailing newline if present (getline keeps it)
        if (re > 0 && le[re-1] == '\n') le[re-1] = '\0';
        if (ra > 0 && la[ra-1] == '\n') la[ra-1] = '\0';
        if (strcmp(le, la) != 0) { differing = 1; break; }
    }
    free(le); free(la);
    fclose(fe); fclose(fa);
    if (differing) { *mismatch_line = line; return 1; }
    return 0;
}

// Helper: write commands of a specific type (first char) to a temp file.
// Returns path to temp file (caller must free string). If none found returns NULL.
char *write_temp_commands_for_type(char **commands, int ncmds, char type, int min_total, int *out_written) {
    // create temp file
    char tmpl[] = "/tmp/programa_test_cmds_XXXXXX";
    int fd = mkstemp(tmpl);
    if (fd < 0) return NULL;
    FILE *f = fdopen(fd, "w");
    if (!f) { close(fd); return NULL; }
    int written = 0;
    int pass = 0;
    int target = (min_total > 0) ? min_total : 1;
    while (written < target) {
        int any_this_pass = 0;
        for (int i = 0; i < ncmds; i++) {
            if (commands[i][0] == type) {
                fprintf(f, "%s\n", commands[i]);
                written++;
                any_this_pass = 1;
                if (written >= target) break;
            }
        }
        pass++;
        if (!any_this_pass) break; // no commands of that type exist
        if (pass > 1000000) break; // paranoia guard
    }
    fclose(f);
    if (written == 0) { unlink(tmpl); return NULL; }
    if (out_written) *out_written = written;
    return strdup(tmpl);
}

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <dataset_dir> <commands_file> <expected_dir> [repeats]\n", argv[0]);
        return 2;
    }
    const char *dataset_dir = argv[1];
    const char *commands_file = argv[2];
    const char *expected_dir = argv[3];
    int repeats = (argc >= 5) ? atoi(argv[4]) : 3;

    char program_dir[4096] = {0};
    char program_path[4096] = {0};
    if (get_dirname_from_argv0(argv[0], program_dir, sizeof(program_dir)) != 0 ||
        join_path(program_path, sizeof(program_path), program_dir, "programa-principal") != 0) {
        fprintf(stderr, "Failed to resolve programa-principal path from argv[0]=%s\n", argv[0]);
        return 2;
    }

    // Load commands
    int ncmds = 0;
    char **commands = read_lines(commands_file, &ncmds);
    if (!commands) { fprintf(stderr, "Failed to read commands file\n"); return 2; }

    // Run program once and measure overall time/memory
    double elapsed = 0.0;
    long peak_kb = 0;
    printf("Running program once to produce outputs...\n");
    if (run_program_and_measure(program_dir, program_path, dataset_dir, commands_file, &elapsed, &peak_kb) != 0) {
        fprintf(stderr, "Failed to run programa-principal\n");
        return 3;
    }
    printf("Overall time: %.3f s, peak mem: %ld KB\n", elapsed, peak_kb);

    // Compare outputs
    int *total_by_type = calloc(10, sizeof(int));
    int *correct_by_type = calloc(10, sizeof(int));
    int *first_mismatch_line = calloc(10, sizeof(int)); // 0 = none

    for (int i = 0; i < ncmds; i++) {
        char type = commands[i][0];
        if (type < '0' || type > '9') continue;
        int q = type - '0';
        total_by_type[q]++;
        char expected_path[4096];
        char actual_path[4096];
        snprintf(expected_path, sizeof(expected_path), "%s/command%d_output.txt", expected_dir, i+1);
        snprintf(actual_path, sizeof(actual_path), "resultados/command%d_output.txt", i+1);
        int first_line = 0;
        if (compare_files_first_mismatch(expected_path, actual_path, &first_line) == 0) {
            correct_by_type[q]++;
        } else {
            if (first_mismatch_line[q] == 0) first_mismatch_line[q] = first_line;
        }
    }

    printf("\nResults by query type:\n");
    for (int q = 0; q < 10; q++) {
        if (total_by_type[q] == 0) continue;
        printf("Query %d: %d / %d correct", q, correct_by_type[q], total_by_type[q]);
        if (first_mismatch_line[q] != 0) printf("  first mismatch at line %d", first_mismatch_line[q]);
        printf("\n");
    }

    // Optional: measure average time per query type
    printf("\nMeasuring average time per query type (repeats=%d)...\n", repeats);
    /* Measure baseline (initialization) time using an empty commands file.
       We'll subtract this from per-type totals to approximate query-only time. */
    char empty_tmpl[] = "/tmp/programa_test_cmds_empty_XXXXXX";
    int empty_fd = mkstemp(empty_tmpl);
    double baseline_avg = 0.0;
    if (empty_fd >= 0) {
        close(empty_fd);
        for (int r = 0; r < repeats; r++) {
            double t; long mem;
            if (run_program_and_measure(program_dir, program_path, dataset_dir, empty_tmpl, &t, &mem) == 0) {
                baseline_avg += t;
            } else {
                fprintf(stderr, "Baseline run failed (repeat %d)\n", r);
            }
        }
        unlink(empty_tmpl);
        baseline_avg /= repeats;
    } else {
        baseline_avg = 0.0;
    }
    for (int q = 0; q < 10; q++) {
        if (total_by_type[q] == 0) continue;
        char type = '0' + q;
        // Repeat commands so the per-query timing isn't dominated by init/noise.
        int min_cmds = 50;
        int written = 0;
        char *tmp_cmds = write_temp_commands_for_type(commands, ncmds, type, min_cmds, &written);
        if (!tmp_cmds) continue;
        double total_t = 0.0;
        for (int r = 0; r < repeats; r++) {
            double t; long mem;
            if (run_program_and_measure(program_dir, program_path, dataset_dir, tmp_cmds, &t, &mem) == 0) {
                total_t += t;
            } else {
                fprintf(stderr, "Run failed for query type %d (repeat %d)\n", q, r);
            }
        }
        unlink(tmp_cmds);
        free(tmp_cmds);
        double avg = total_t / repeats;
        double query_only = avg - baseline_avg;
        if (query_only < 0) query_only = 0.0;
        double per_query = (written > 0) ? (query_only / written) : 0.0;
        printf("Query %d avg (run): %.3f s | baseline: %.3f s | query-only: %.3f s | per-query: %.6f s (n=%d)\n",
               q, avg, baseline_avg, query_only, per_query, written);
    }

    // Cleanup
    for (int i = 0; i < ncmds; i++) free(commands[i]);
    free(commands);
    free(total_by_type); free(correct_by_type); free(first_mismatch_line);

    return 0;
}
