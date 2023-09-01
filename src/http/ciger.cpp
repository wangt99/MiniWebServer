#include "http/ciger.h"
#include "http/http_utils.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <exception>
#include <thread>
#include <vector>
#include <cassert>
#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

namespace pine::http {

Cgier::Cgier(const std::string &path, const std::vector<std::string> &args) noexcept
    : cgi_path_(path), cgi_arguments_(args), valid_(true) {}

auto Cgier::Run() -> std::vector<unsigned char> {
    if (!valid_) {
        std::cerr << "cgier is invalid.\n";
        std::terminate();
    }
    // cgi程序返回结果
    std::vector<unsigned char> result;
    std::stringstream ss;
    ss << CGI_PREFIX << UNDERSCORE << std::this_thread::get_id() << ".txt";
    auto shared_file_name = ss.str();
    int fd = open(shared_file_name.c_str(), O_RDWR | O_APPEND | O_CREAT, READ_WRITE_PERM);
    if (fd == -1) {
        std::string err_msg = "fail to create/open the file " + shared_file_name;
        return {err_msg.begin(), err_msg.end()};
    }
    pid_t pid = fork();
    if (pid == -1) {
        std::string err = "fail to fork";
        return {err.begin(), err.end()};
    }
    if (pid == 0) { // 子进程
        // 将输出连接到共享文件中
        dup2(fd, STDOUT_FILENO);
        close(fd);
        // 在子进程中允许cgi程序
        char **argv = BuildArgumentList();
        if (execve(cgi_path_.c_str(), argv, NULL) < 0) {
            perror("failed to execve");
            FreeArgumentList(argv);
            exit(1);
        }
    } else { // 父进程
        close(fd);
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            std::string error = "fail to harvest child by waitpid()";
            return {error.begin(), error.end()};
        }
        // 从共享文件中加载结果
        LoadFile(shared_file_name, result);
        // 删除文件
        DeleteFile(shared_file_name);
    }
    return result;
} 

auto Cgier::ParseCgier(const std::string &resource_url) noexcept -> Cgier {
    if (resource_url.empty() || !IsCgiRequest(resource_url)) {
        return MakeInvalidCgier();
    }
    // find the first & after the cgi-bin/ to fetch out cgi program path
    auto cgi_pos = resource_url.find(CGI_BIN);
    auto cgi_sep = resource_url.find(PARAMETER_SEPARATOR, cgi_pos);
    auto cgi_path = resource_url.substr(0, cgi_sep - cgi_pos);
    auto arguments = Split(resource_url.substr(cgi_sep + 1), PARAMETER_SEPARATOR);
    return Cgier(cgi_path, arguments);
}

auto Cgier::MakeInvalidCgier() noexcept -> Cgier {
    auto invalid_cgier = Cgier();
    return invalid_cgier;
}

// private helper functuion
void Cgier::FreeArgumentList(char **arg_list) {
    for (size_t i = 0; i < cgi_arguments_.size() + 2; i++) {
        free(arg_list[i]);
    }
    free(arg_list);
}

auto Cgier::BuildArgumentList() -> char ** {
    assert(!cgi_path_.empty());
    char **cgi_args = (char**)calloc(cgi_arguments_.size() + 2, sizeof(char *));
    cgi_args[0] = (char *)calloc(cgi_path_.size() + 1, sizeof(char));
    memcpy(cgi_args[0], cgi_path_.data(), cgi_path_.size());
    cgi_args[0][cgi_path_.size() + 1] = '\0';
    for (size_t i = 0; i < cgi_arguments_.size(); i++) {
        cgi_args[i+1] = (char *)calloc(cgi_arguments_[i].size() + 1, sizeof(char));
        memcpy(cgi_args[i+1], cgi_arguments_[i].data(), cgi_arguments_[i].size());
        cgi_args[i+1][cgi_arguments_.size() + 1] = '\0';
    }
    cgi_args[cgi_arguments_.size() + 1] = nullptr;
    return cgi_args;
}

}