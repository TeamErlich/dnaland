#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "Cache.h"

using namespace std;
using namespace leveldb;

namespace Cache {

        DB *cache;
        ReadOptions cache_ro;
        WriteOptions cache_wo;

        void init(const string& cache_dir) {
                Options opts;
                opts.filter_policy = NewBloomFilterPolicy(16);
                opts.create_if_missing = true;
                Status status = DB::Open(opts, cache_dir, &cache);
                if (!status.ok()) {
                        cerr << "Failure opening cache db in \"" << cache_dir
                             << "\": " << status.ToString() << endl;
                        exit(1);
                }

                // Basic Sanity Check
                int twofiftyeight = 258;
                string buf;
                buf.resize(sizeof(int));
                memcpy(&buf[0], &twofiftyeight, sizeof(int));
                string key="twofiftyeight";
                string val=get(key);
                if (val=="") {
                        // New Cache
                        put(key, buf);
                } else if (val!=buf) {
                        cerr << "Cache is for a different architechture than "
                             << "current CPU: the number 258 is encoded as 0x";
                        for (int i : val) cerr << hex << setfill('0') << setw(2) << i;
                        cerr << " whereas this CPU uses 0x";
                        for (int i : buf) cerr << hex << setfill('0') << setw(2) << i;
                        cerr << endl;
                        exit(1);
                }
        }

        bool isInit() {
                return !!cache;
        }

        string get(const string& key) {
                if (!cache) return "";
                string val;
                Status status = cache->Get(cache_ro, key, &val);
                if (status.ok()) {
                        return val;
                } else {
                        return "";
                }
        }

        void put(const string& key, const string& val) {
                if (!cache) return;
                Status status = cache->Put(cache_wo, key, val);
                if (!status.ok()) {
                        cerr << "Failure writing to cache key=\"" << key
                             << "\": " << status.ToString() << endl;
                        exit(1);
                }
        }

        void cleanup() {
                if (cache) delete cache;
        }
};
