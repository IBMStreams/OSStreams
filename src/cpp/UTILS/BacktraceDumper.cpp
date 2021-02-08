/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if (defined(__i386__) || defined(__x86_64) || defined(__PPC__) || defined(__PPC64__)) &&          \
  __linux__ && __GNUC__ >= 3

#include <cxxabi.h>
#include <execinfo.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif

#include <UTILS/BacktraceDumper.h>
#include <UTILS/SupportFunctions.h>

using namespace std;

UTILS_NAMESPACE_USE

#ifdef DUMP_LINE_NUMBER

void BacktraceDumper::findAddress(bfd* abfd, asection* section, void* data)
{
    find_context_t* ctx = (find_context_t*)data;

    // fprintf(stderr, " ++++ %s | %x | %d\n", section->name, section->vma, section->size) ;

    if (ctx->found) {
        return;
    }

    bfd_vma vma;

    if ((bfd_get_section_flags(abfd, section) & SEC_ALLOC) == 0) {
        return;
    }

    vma = bfd_get_section_vma(abfd, section);
    if (ctx->pc < vma) {
        return;
    }

    if (ctx->pc >= vma + section->size) {
        return;
    }

    const char* xx = NULL;

    ctx->found = bfd_find_nearest_line(abfd, section, ctx->syms, ctx->pc - vma, &(ctx->filename),
                                       &xx, &(ctx->line));
}

#endif

int BacktraceDumper::dump(char* user_buffer,
                          int max_size,
                          int& final_size,
                          bool add_lines,
                          int skip)
{
    int offset = 0;

#define BUF_PRINTF(fmt, args...)                                                                   \
    do {                                                                                           \
        if (offset < max_size) {                                                                   \
            int size = snprintf(user_buffer + offset, max_size - offset, fmt, ##args);             \
            offset = std::min(offset + size, max_size);                                            \
        }                                                                                          \
    } while (0)

#if (defined(__i386__) || defined(__x86_64) || defined(__PPC__)) && __linux__ &&                   \
  !defined SPC_NO_BACKTRACE

    void* bt_array[STACK_TRACE_SIZE];
    char** bt_strings;
    int num_entries;

    if ((num_entries = backtrace(bt_array, STACK_TRACE_SIZE)) < 0) {
        // fdumpf(stderr, "* Could not generate backtrace\n");
        return 1;
    }

    if ((bt_strings = backtrace_symbols(bt_array, num_entries)) == NULL) {
        // fdumpf(stderr, "* Could not get symbol names for backtrace\n");
        return 1;
    }

#ifdef DUMP_LINE_NUMBER

    bfd* abfd = NULL;
    asymbol** syms = NULL;
    union syms_union_t
    {
        asymbol*** ptr;
        void** raw;
    } syms_union;

    if (add_lines) {
        struct stat st;
        char filename[] = "/proc/self/exe";
        if (stat(filename, &st)) {
            add_lines = false;
        } else {
            abfd = bfd_openr(filename, NULL /* default */);
            if (abfd == NULL) {
                add_lines = false;
            } else {
                char** matching;

                if (!bfd_check_format_matches(abfd, bfd_object, &matching)) {
                    add_lines = false;
                } else {
                    if ((bfd_get_file_flags(abfd) & HAS_SYMS) == 0) {
                        add_lines = false;
                    } else {
                        long symcount;
                        unsigned int size;
                        syms_union.ptr = &syms;

                        symcount = bfd_read_minisymbols(abfd, FALSE, syms_union.raw, &size);

                        if (symcount == 0) {
                            symcount =
                              bfd_read_minisymbols(abfd, TRUE /* dynamic */, syms_union.raw, &size);
                        }

                        if (symcount < 0) {
                            add_lines = false;
                        }
                    }

                    free(matching);
                }
            }
        }
    }

#endif

    for (int i = skip; i < num_entries && offset < max_size; i++) {
        char buffer[strlen(bt_strings[i]) + 1];
        char* result;
        int status;

        char* ptr1 = strchr(bt_strings[i], '(');
        char* ptr2 = strchr(bt_strings[i], '+');

        if (ptr1 && ptr2) {
            *ptr1 = 0;
            *ptr2 = 0;

            strcpy(buffer, ptr1 + 1);

            result = abi::__cxa_demangle(buffer, NULL, NULL, &status);

            if (status == 0) {
                BUF_PRINTF("%s(%s+%s", bt_strings[i], result, ptr2 + 1);
                free(result);
            } else {
                *ptr1 = '(';
                *ptr2 = '+';
                BUF_PRINTF("%s", bt_strings[i]);
            }
        } else {
            BUF_PRINTF("%s", bt_strings[i]);
        }

#ifdef DUMP_LINE_NUMBER

        if (add_lines) {
            find_context_t ctx;
            memset(&ctx, 0, sizeof(ctx));

            ctx.syms = syms;
            ctx.pc = (unsigned long)bt_array[i];

            bfd_map_over_sections(abfd, findAddress, &ctx);

            if (!ctx.found) {
                BUF_PRINTF(" ??");
            } else {
                if (ctx.filename) {
                    BUF_PRINTF(" %s:%u", ctx.filename, ctx.line);
                } else {
                    BUF_PRINTF(" ??");
                }
            }
        }

#endif

        if (i < num_entries - 1) {
            BUF_PRINTF("\n");
        }
    }

#ifdef DUMP_LINE_NUMBER

    if (syms) {
        free(syms);
    }

    if (abfd) {
        bfd_close(abfd);
    }

#endif

    free(bt_strings);

#else

    BUF_PRINTF("no backtrace support");

#endif

#undef BUF_PRINTF

    final_size = offset;

    return 0;
}

int BacktraceDumper::dump(ostream& s, bool add_lines, int skip)
{
    char buffer[MAX_STACK_TRACE_SIZE * 2];
    int final_size;
    int res = dump(buffer, MAX_STACK_TRACE_SIZE, final_size, add_lines, skip + 1);
    buffer[MAX_STACK_TRACE_SIZE] = '\0';

    if (res == 0) {
        s << buffer;
    }

    return res;
}

int BacktraceDumper::dump(int fd, bool add_lines, int skip)
{
    char buffer[MAX_STACK_TRACE_SIZE * 2];
    int final_size;
    int res = dump(buffer, MAX_STACK_TRACE_SIZE, final_size, add_lines, skip + 1);
    buffer[MAX_STACK_TRACE_SIZE] = '\0';

    if (res == 0) {
        if (final_size == MAX_STACK_TRACE_SIZE) {
            final_size = MAX_STACK_TRACE_SIZE - 1;
        }
        write(fd, buffer, final_size);
    }

    return res;
}
