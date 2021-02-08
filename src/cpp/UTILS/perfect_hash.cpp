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

#include <iostream>
#include <map>
using namespace std;

int main(int argc, char** argv)
{
    int max_len = 0;

    for (int i = 1; i < argc; i++) {
        int l = strlen(argv[i]);
        if (l > max_len) {
            max_len = l;
        }
    }

    for (int k1 = 0; k1 < max_len; k1++) {
        for (int k2 = 0; k2 < max_len; k2++) {
            for (int k3 = 0; k3 < max_len; k3++) {
                map<int, int> m;

                bool collision = false;

                for (int i = 1; i < argc; i++) {
                    int len = strlen(argv[i]);
                    int h = argv[i][k1 % len] + argv[i][k2 % len] + argv[i][k3 % len];

                    if (m[h] != 0) {
                        collision = true;
                    }

                    m[h] = 1;

                    // cout << h << " " ;
                }

                if (!collision) {
                    cout << " k1=" << k1 << " k2=" << k2 << " k3=" << k3 << endl;

                    int min_value = INT_MAX;
                    int max_value = 0;

                    for (int i = 1; i < argc; i++) {
                        int l = strlen(argv[i]);
                        int h = argv[i][k1 % l] + argv[i][k2 % l] + argv[i][k3 % l];

                        if (h < min_value) {
                            min_value = h;
                        }

                        if (h > max_value) {
                            max_value = h;
                        }
                    }

                    max_value = max_value - min_value;

                    map<int, int> m2;

                    for (int i = 1; i < argc; i++) {
                        int l = strlen(argv[i]);
                        int h = argv[i][k1 % l] + argv[i][k2 % l] + argv[i][k3 % l];

                        m2[h - min_value] = i;

                        // cout << h - min_value << " [" << max_value << "]  -- " << argv[i] << endl
                        // ;
                    }

                    cout << "NULL," << endl;
                    for (int i = 1; i < argc; i++) {
                        cout << "/* " << i << " */ \"" << argv[i] << "\"," << endl;
                    }

                    cout << "#define MIN_HASH_VALUE " << min_value << endl;
                    cout << "#define MAX_HASH_VALUE " << max_value << endl;

                    for (int i = 0; i <= max_value; i++) {
                        if (i && (i % 16) == 0) {
                            cout << endl;
                        }

                        cout << m2[i] << ", ";
                    }

                    cout << endl;

                    return 0;
                }
            }
        }
    }

    return 0;
}
