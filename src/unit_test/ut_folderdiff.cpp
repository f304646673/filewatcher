/*************************************************************************
    > File Name: ut_folderdiff.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Thu 31 Jan 2019 11:04:00 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <boost/filesystem.hpp>

#include "utils/file_util.h"

#include "watcher/folderdiff.h"

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace filewatcher {

using filewatcher::PathType;
using filewatcher::PathInfo;
using filewatcher::PathInfoSet;
using filewatcher::FolderDiff;

class UtFolderDiff :
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

TEST_F(UtFolderDiff, CreateFolder) {
    std::string path = root_ + "/creatfolder";
    create_folder(path);

    FolderDiff ff(path);

    std::string create_folder_path = path + "/tempfolder";
    boost::filesystem::create_directory(create_folder_path);

    PathInfoSet add;
    PathInfoSet remove;
    ff.diff(add, remove);
    for_each(add.begin(), add.end(), 
        [create_folder_path](const PathInfo& pi) {
            ASSERT_STREQ(create_folder_path.c_str(), pi.path.c_str());}
    );

    ASSERT_EQ(add.size(), 1);
    ASSERT_EQ(remove.size(), 0);
}

TEST_F(UtFolderDiff, RemoveFolder) {
    std::string path = root_ + "/removefolder";
    create_folder(path);
    std::string remove_folder_path = path + "/tempfolder";
    boost::filesystem::create_directory(remove_folder_path);

    FolderDiff ff(path);

    boost::filesystem::remove_all(remove_folder_path);

    PathInfoSet add;
    PathInfoSet remove;
    ff.diff(add, remove);
    for_each(remove.begin(), remove.end(), 
        [remove_folder_path](const PathInfo& pi) {
            ASSERT_STREQ(remove_folder_path.c_str(), pi.path.c_str());}
    );

    ASSERT_EQ(remove.size(), 1);
    ASSERT_EQ(add.size(), 0);
}

TEST_F(UtFolderDiff, CreateFile) {
    std::string path = root_ + "/creatfile";
    create_folder(path);

    FolderDiff ff(path);

    std::string create_file_path = path + "/tempfile";
    boost::filesystem::ofstream(create_file_path.c_str());

    PathInfoSet add;
    PathInfoSet remove;
    ff.diff(add, remove);
    for_each(add.begin(), add.end(), 
        [create_file_path](const PathInfo& pi) {
            ASSERT_STREQ(create_file_path.c_str(), pi.path.c_str());}
    );

    ASSERT_EQ(add.size(), 1);
    ASSERT_EQ(remove.size(), 0);
}

TEST_F(UtFolderDiff, RemoveFile) {
    std::string path = root_ + "/creat";
    create_folder(path);
    std::string remove_file_path = path + "/tempfile";
    boost::filesystem::ofstream(remove_file_path.c_str());

    FolderDiff ff(path);

    boost::filesystem::remove(remove_file_path);

    PathInfoSet add;
    PathInfoSet remove;
    ff.diff(add, remove);
    for_each(remove.begin(), remove.end(), 
        [remove_file_path](const PathInfo& pi) {
            ASSERT_STREQ(remove_file_path.c_str(), pi.path.c_str());
            }
    );
    
    ASSERT_EQ(add.size(), 0);
    ASSERT_EQ(remove.size(), 1);
}

}
