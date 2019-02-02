/*************************************************************************
    > File Name: ut_filewatcher.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 05:12:08 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <boost/filesystem.hpp>

#include "utils/file_util.h"
#include "watcher/filewatcher.h"

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace filewatcher {

using filewatcher::FileWatcher;
using filewatcher::FileWatcherAction;
using filewatcher::FileWatcherAction;

class UtFileWatcher :
    public ::testing::Test 
{
protected:
    virtual void SetUp() {
		root_ = boost::filesystem::current_path().string() + "/TestCase";
        create_folder(root_);
    }
    virtual void TearDown() {
        if (boost::filesystem::exists(root_)) {
            boost::filesystem::remove_all(root_);
        }       
    }
public:
    std::string root_;
    int wait_milliseconds_ = 1000;
};

TEST_F(UtFileWatcher, Create) {
    std::string create_path = root_ + "/creat";
    create_folder(create_path);
    std::string create_file_path = create_path + "/temp";

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(create_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(create_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(create_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_NEW);
}

TEST_F(UtFileWatcher, Remove) {
    std::string remove_path = root_ + "/remove";
    create_folder(remove_path);
    std::string remove_file_path = remove_path + "/temp";
    boost::filesystem::ofstream(remove_file_path.c_str());

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(remove_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::remove(remove_file_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(remove_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_DEL);
}

TEST_F(UtFileWatcher, Rename) {
    std::string rename_path = root_ + "/rename";
    create_folder(rename_path);
    std::string rename_file_src_path = rename_path + "/temp";
    boost::filesystem::ofstream(rename_file_src_path.c_str());
    std::string rename_file_dst_path = rename_file_src_path + "1";

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(rename_file_src_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::rename(rename_file_src_path, rename_file_dst_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(rename_file_src_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_DEL);
}

TEST_F(UtFileWatcher, Copy) {
    std::string copy_path = root_ + "/copy";
    create_folder(copy_path);
    std::string dst_file_path = copy_path + "/temp_dst";
    
    std::string src_file_path = copy_path + "/temp_src";
    boost::filesystem::ofstream(src_file_path.c_str());

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(dst_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::copy_file(src_file_path, dst_file_path, boost::filesystem::copy_option::overwrite_if_exists);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(dst_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_NEW);
}

TEST_F(UtFileWatcher, CopyWatchSrc) {
    std::string copy_watch_src_path = root_ + "/copy_watch_src";
    create_folder(copy_watch_src_path);
    std::string dst_file_path = copy_watch_src_path + "/temp_dst";
    
    std::string src_file_path = copy_watch_src_path + "/temp_src";
    boost::filesystem::ofstream(src_file_path.c_str());

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(src_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::copy_file(src_file_path, dst_file_path, boost::filesystem::copy_option::overwrite_if_exists);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(callback_path.length(), 0);
    ASSERT_EQ(fac, FILE_UNKNOWN);
}

TEST_F(UtFileWatcher, Write) {
    std::string write_path = root_ + "/write";
    create_folder(write_path);
    std::string write_patht_file_path = write_path + "/temp";
    boost::filesystem::ofstream(write_patht_file_path.c_str());

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(write_patht_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_src(new boost::filesystem::ofstream(write_patht_file_path.c_str()));
        temp_src->write(write_patht_file_path.c_str(), write_patht_file_path.length());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(write_patht_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_MODIFY);
}

TEST_F(UtFileWatcher, OverWrite) {
    std::string overwrite_path = root_ + "/overwrite";
    create_folder(overwrite_path);
    std::string dst_file_path = overwrite_path + "/temp_dst";
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_dst(new boost::filesystem::ofstream(dst_file_path.c_str()));
        temp_dst->write(dst_file_path.c_str(), dst_file_path.length());
    }
    
    std::string src_file_path = overwrite_path + "/temp_src";
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_src(new boost::filesystem::ofstream(src_file_path.c_str()));
        temp_src->write(src_file_path.c_str(), src_file_path.length());
    }

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(dst_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::copy_file(src_file_path, dst_file_path, boost::filesystem::copy_option::overwrite_if_exists);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(dst_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_MODIFY);
}

TEST_F(UtFileWatcher, CustomLoop) {
    std::string custom_loop_path = root_ + "/custom_loop";
    create_folder(custom_loop_path);
    std::string create_file_path = custom_loop_path + "/temp";

    std::string callback_path;
    FileWatcherAction fac = FILE_UNKNOWN;
    FileWatcher fw(create_file_path, 
        [&callback_path, &fac](const std::string& path, FileWatcherAction action) {
            callback_path = path;fac = action;
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(create_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_STREQ(create_file_path.c_str(), callback_path.c_str());
    ASSERT_EQ(fac, FILE_NEW);
}

TEST_F(UtFileWatcher, DoubleWatch) {
    std::string doublewatch_path = root_ + "/doublewatch";
    create_folder(doublewatch_path);
    std::string doublewatch_file_path = doublewatch_path + "/temp";

    int count = 0;
    struct FileInfo {
        std::string path;
        FileWatcherAction fac;
    };
    std::vector<FileInfo> fileinfo_vec;
    FileWatcher fw_1(doublewatch_file_path, 
        [&fileinfo_vec, &count](const std::string& path, FileWatcherAction action) {
            FileInfo fi{path, action};
            fileinfo_vec.push_back(fi);
            }
    );

    FileWatcher fw_2(doublewatch_file_path, 
        [&fileinfo_vec, &count](const std::string& path, FileWatcherAction action) {
            FileInfo fi{path, action};
            fileinfo_vec.push_back(fi);
            }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(doublewatch_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    for_each(fileinfo_vec.begin(), fileinfo_vec.end(), 
        [doublewatch_file_path](const FileInfo& fi) {
            ASSERT_STREQ(fi.path.c_str(), doublewatch_file_path.c_str());
            ASSERT_EQ(fi.fac, FILE_NEW);
            }
    );
    ASSERT_EQ(fileinfo_vec.size(), 2);
}

}
