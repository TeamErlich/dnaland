#include <string>

using namespace std;

namespace Cache {

        // Set up caching using the directory given
        // If the directory does not exist, create it
        // exit(1) on error
        // If put, get or cleanup are called without this
        // they become no-ops
        void init(const string& cache_dir);

        // Check to see if we've been initialized
        bool isInit();

        // Read from cache.
        // Returns empty string if key not found or error
        string get(const string& key);

        // Write to cache.  exit(1) on error
        void put(const string& key, const string& val);

        // Must be called before program termination or
        // some values may not be flushed to disk
        void cleanup();

};
