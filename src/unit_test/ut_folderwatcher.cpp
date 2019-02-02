/*************************************************************************
    > File Name: ut_folderwatcher.cpp
    > Author: fangliang
    > Mail: f304646673@gmail.com 
    > Created Time: Fri 25 Jan 2019 08:14:02 PM CST
 ************************************************************************/
#include <gtest/gtest.h>
#include <set>
#include <memory>
#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <boost/filesystem.hpp>

#include "utils/file_util.h"
#include "watcher/folderwatcher.h"

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace filewatcher {

using filewatcher::LibevLoop;
using filewatcher::FolderWatcher;

class UtFolderWatcher :
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

struct PathInfoAndAction {
    PathInfo pi;
    WatcherAction action;
    bool operator < (const PathInfoAndAction & right) const {
        return pi.path.compare(right.pi.path) < 0;
    }
};

TEST_F(UtFolderWatcher, CreateFile) {
    std::string path = root_ + "/creat_file";
    create_folder(path);
    std::string create_file_path = path + "/temp";
    
    PathInfoAndAction pic;
    std::vector<PathInfoAndAction> pic_vec;

    FolderWatcher fw(path, 
        [&pic_vec](const PathInfo& pi, WatcherAction action) {
            pic_vec.push_back(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(create_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    for_each(pic_vec.begin(), pic_vec.end(), 
        [create_file_path](const PathInfoAndAction& pic) {
            ASSERT_STREQ(create_file_path.c_str(), pic.pi.path.c_str());
            ASSERT_EQ(pic.pi.type, E_FILE);
            ASSERT_EQ(pic.action, NEW);
            }
    );

    ASSERT_EQ(pic_vec.size(), 1);
}

TEST_F(UtFolderWatcher, CreateFolder) {
    std::string path = root_ + "/creat_folder";
    create_folder(path);
    std::string create_folder_path = path + "/temp";
    
    PathInfoAndAction pic;
    std::vector<PathInfoAndAction> pic_vec;

    FolderWatcher fw(path, 
        [&pic_vec](const PathInfo& pi, WatcherAction action) {
            pic_vec.push_back(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    create_folder(create_folder_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    for_each(pic_vec.begin(), pic_vec.end(), 
        [create_folder_path](const PathInfoAndAction& pic) {
            ASSERT_STREQ(create_folder_path.c_str(), pic.pi.path.c_str());
            ASSERT_EQ(pic.pi.type, E_FOLDER);
            ASSERT_EQ(pic.action, NEW);
            }
    );

    ASSERT_EQ(pic_vec.size(), 1);
}

TEST_F(UtFolderWatcher, RemoveFile) {
    std::string path = root_ + "/remove_file";
    create_folder(path);
    std::string remove_file_path = path + "/temp";
    boost::filesystem::ofstream(remove_file_path.c_str());

    PathInfoAndAction pic;
    std::vector<PathInfoAndAction> pic_vec;

    FolderWatcher fw(path, 
        [&pic_vec](const PathInfo& pi, WatcherAction action) {
            pic_vec.push_back(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::remove(remove_file_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    for_each(pic_vec.begin(), pic_vec.end(), 
        [remove_file_path](const PathInfoAndAction& pic) {
            //std::cout << "remove: " << pic.pi.path << std::endl;
            ASSERT_STREQ(remove_file_path.c_str(), pic.pi.path.c_str());
            ASSERT_EQ(pic.pi.type, E_FILE);
            ASSERT_EQ(pic.action, DEL);
            }
    );

    ASSERT_EQ(pic_vec.size(), 1);
}

TEST_F(UtFolderWatcher, RemoveFolder) {
    std::string path = root_ + "/remove_folder";
    create_folder(path);
    std::string remove_folder_path = path + "/temp";
    create_folder(remove_folder_path);

    PathInfoAndAction pic;
    std::vector<PathInfoAndAction> pic_vec;

    FolderWatcher fw(path, 
        [&pic_vec](const PathInfo& pi, WatcherAction action) {
            pic_vec.push_back(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::remove(remove_folder_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    for_each(pic_vec.begin(), pic_vec.end(), 
        [remove_folder_path](const PathInfoAndAction& pic) {
            //std::cout << "remove: " << pic.pi.path << std::endl;
            ASSERT_STREQ(remove_folder_path.c_str(), pic.pi.path.c_str());
            ASSERT_EQ(pic.pi.type, E_FOLDER);
            ASSERT_EQ(pic.action, DEL);
            }
    );

    ASSERT_EQ(pic_vec.size(), 1);
}

TEST_F(UtFolderWatcher, RenameFile) {
    std::string path = root_ + "/rename_file";
    create_folder(path);
    std::string rename_src_path = path + "/temp";
    boost::filesystem::ofstream(rename_src_path.c_str());
    std::string rename_dst_path = rename_src_path + "1";

    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::rename(rename_src_path, rename_dst_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    auto it_s = pic_set.find(PathInfoAndAction{PathInfo{rename_src_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_s, pic_set.end());
    ASSERT_EQ(it_s->action, DEL);
    ASSERT_EQ(it_s->pi.type, E_FILE);

    auto it_d = pic_set.find(PathInfoAndAction{PathInfo{rename_dst_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_d, pic_set.end());
    ASSERT_EQ(it_d->action, NEW);
    ASSERT_EQ(it_d->pi.type, E_FILE);

    ASSERT_EQ(pic_set.size(), 2);
}

TEST_F(UtFolderWatcher, RenameFolder) {
    std::string path = root_ + "/rename_folder";
    create_folder(path);
    std::string rename_src_path = path + "/temp";
    create_folder(rename_src_path);
    std::string rename_dst_path = rename_src_path + "1";

    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::rename(rename_src_path, rename_dst_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    auto it_s = pic_set.find(PathInfoAndAction{PathInfo{rename_src_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_s, pic_set.end());
    ASSERT_EQ(it_s->action, DEL);
    ASSERT_EQ(it_s->pi.type, E_FOLDER);

    auto it_d = pic_set.find(PathInfoAndAction{PathInfo{rename_dst_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_d, pic_set.end());
    ASSERT_EQ(it_d->action, NEW);
    ASSERT_EQ(it_d->pi.type, E_FOLDER);

    ASSERT_EQ(pic_set.size(), 2);
}

TEST_F(UtFolderWatcher, CopyFile) {
    std::string path = root_ + "/copy_file";
    create_folder(path);
    std::string dst_file_path = path + "/temp_dst";
    
    std::string src_file_path = path + "/temp_src";
    boost::filesystem::ofstream(src_file_path.c_str());

    PathInfoAndAction pic;
    std::vector<PathInfoAndAction> pic_vec;

    FolderWatcher fw(path, 
        [&pic_vec](const PathInfo& pi, WatcherAction action) {
            pic_vec.push_back(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::copy_file(src_file_path, dst_file_path, boost::filesystem::copy_option::overwrite_if_exists);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    for_each(pic_vec.begin(), pic_vec.end(), 
        [dst_file_path](const PathInfoAndAction& pic) {
            ASSERT_STREQ(dst_file_path.c_str(), pic.pi.path.c_str());
            ASSERT_EQ(pic.pi.type, E_FILE);
            ASSERT_EQ(pic.action, NEW);
            }
    );

    ASSERT_EQ(pic_vec.size(), 1);
}

TEST_F(UtFolderWatcher, OpenFile) {
    std::string path = root_ + "/open_file";
    create_folder(path);
    std::string open_file_path = path + "/temp";
    boost::filesystem::ofstream(open_file_path.c_str());

    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_dst(new boost::filesystem::ofstream(open_file_path.c_str()));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(pic_set.size(), 1);

    auto it = pic_set.find(PathInfoAndAction{PathInfo{open_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it, pic_set.end());
    ASSERT_EQ(it->pi.type, E_FILE);
    ASSERT_EQ(it->action, MODIFY);
}

TEST_F(UtFolderWatcher, Write) {
    std::string path = root_ + "/write";
    create_folder(path);
    std::string write_file_path = path + "/temp";
    boost::filesystem::ofstream(write_file_path.c_str());

    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_dst(new boost::filesystem::ofstream(write_file_path.c_str()));
        temp_dst->write(write_file_path.c_str(), write_file_path.length());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(pic_set.size(), 2);

    auto it = pic_set.find(PathInfoAndAction{PathInfo{write_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it, pic_set.end());
    ASSERT_EQ(it->pi.type, E_FILE);
    ASSERT_EQ(it->action, MODIFY);
}

TEST_F(UtFolderWatcher, Coverage) {
    std::string path = root_ + "/coverage";
    create_folder(path);
    std::string dst_file_path = path + "/temp_dst";
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_dst(new boost::filesystem::ofstream(dst_file_path.c_str()));
        temp_dst->write(dst_file_path.c_str(), dst_file_path.length());
    }
    
    std::string src_file_path = path + "/temp_src";
    {
        std::unique_ptr<boost::filesystem::ofstream> temp_src(new boost::filesystem::ofstream(src_file_path.c_str()));
        temp_src->write(src_file_path.c_str(), src_file_path.length());
    }

    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::copy_file(src_file_path, dst_file_path, boost::filesystem::copy_option::overwrite_if_exists);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(pic_set.size(), 1);

    auto it = pic_set.find(PathInfoAndAction{PathInfo{dst_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it, pic_set.end());
    ASSERT_EQ(it->action, MODIFY);
    ASSERT_EQ(it->pi.type, E_FILE);
}

TEST_F(UtFolderWatcher, CreateRemoveCreateFile) {
    std::string path = root_ + "/creat_remove_creat_file";
    create_folder(path);
    std::string create_file_path = path + "/temp";
    
    PathInfoAndAction pic;
    std::multiset<PathInfoAndAction> pic_set;

    FolderWatcher fw(path, 
        [&pic_set](const PathInfo& pi, WatcherAction action) {
            pic_set.insert(PathInfoAndAction{pi, action});
        }
    );

    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(create_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(pic_set.size(), 1);

    auto it = pic_set.find(PathInfoAndAction{PathInfo{create_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it, pic_set.end());
    ASSERT_EQ(it->action, NEW);
    ASSERT_EQ(it->pi.type, E_FILE);

    pic_set.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::remove(create_file_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    ASSERT_EQ(pic_set.size(), 1);

    auto it_r = pic_set.find(PathInfoAndAction{PathInfo{create_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_r, pic_set.end());
    ASSERT_EQ(it_r->action, DEL);
    ASSERT_EQ(it_r->pi.type, E_FILE);

    pic_set.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));
    boost::filesystem::ofstream(create_file_path.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_milliseconds_));

    auto it_c = pic_set.find(PathInfoAndAction{PathInfo{create_file_path, E_FILE}, UNKNOWN});
    ASSERT_NE(it_c, pic_set.end());
    ASSERT_EQ(it_c->action, NEW);
    ASSERT_EQ(it_c->pi.type, E_FILE);
}

}
