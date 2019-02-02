rm -rf output
mkdir output
g++ -o output/folderdiff src/utils/*.cpp src/watcher/*.cpp src/unit_test/ut_folderdiff.cpp -I/usr/local/include/ -Isrc/ -L /usr/local/lib -lev -lgtest -lgtest_main -lboost_serialization -lboost_filesystem -lboost_serialization -lglog -lpthread
g++ -o output/folderwatcher src/utils/*.cpp src/watcher/*.cpp src/unit_test/ut_folderwatcher.cpp -I/usr/local/include/ -Isrc/ -L /usr/local/lib -lev -lgtest -lgtest_main -lboost_serialization -lboost_filesystem -lboost_serialization -lglog -lpthread
g++ -o output/filewatcher src/utils/*.cpp src/watcher/*.cpp src/unit_test/ut_filewatcher.cpp -I/usr/local/include/ -Isrc/ -L /usr/local/lib -lev -lgtest -lgtest_main -lboost_serialization -lboost_filesystem -lboost_serialization -lglog -lpthread
