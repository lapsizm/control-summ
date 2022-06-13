#include <filesystem>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

std::string DoCKSUM(const std::string &p, const std::string& var) {
    std::ifstream fin("../temp.txt", std::ios::out);
    if(var.empty() || var == "md5"){
        system(("md5sum " + p + " > ../temp.txt").c_str());
    }
    else if(var == "sha1"){
        system(("sha1sum " + p + " > ../temp.txt").c_str());
    }
    else if(var == "cksum"){
        system(("cksum " + p + " > ../temp.txt").c_str());
    }
    std::string str;
    std::getline(fin, str);
    int a = str.find(' ', 0);
    str.erase(a, str.length() - a);
    fin.close();
    return str;
}


std::string GetOwner(const std::string &p){
    std::ifstream fin("../temp.txt");
    system(("stat -c%U > ../temp.txt " + p).c_str());
    std::string str;
    std::getline(fin, str);
    fin.close();
    return str;
}

void Print(const fs::path &p, std::ostream &os) {
    if (!is_directory(p)) {
        os << "Name: " << p.filename().string() << std::endl;
        os << "Size of file: ";
        int n = 0;
        double sz = fs::file_size(p);
        while (sz >= 1024) {
            sz /= 1024;
            ++n;
        }
        sz = std::ceil(sz * 10) / 10;
        std::string str = "BKMT";
        os << sz << str[n];
        if (n != 0) {
            os << "B (" << fs::file_size(p) << ")";
        }
        os << "\n";

        std::filesystem::file_time_type ftime = fs::last_write_time(p);
        std::time_t cftime = std::chrono::system_clock::to_time_t(
                std::chrono::file_clock::to_sys(ftime));
        os << "File write time is " << std::asctime(std::localtime(&cftime));

        os << "Created file with permissions: ";
        fs::perms p_ = fs::status(p).permissions();
        os << ((p_ & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
           << ((p_ & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
           << ((p_ & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
           << ((p_ & fs::perms::group_read) != fs::perms::none ? "r" : "-")
           << ((p_ & fs::perms::group_write) != fs::perms::none ? "w" : "-")
           << ((p_ & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
           << ((p_ & fs::perms::others_read) != fs::perms::none ? "r" : "-")
           << ((p_ & fs::perms::others_write) != fs::perms::none ? "w" : "-")
           << ((p_ & fs::perms::others_exec) != fs::perms::none ? "x" : "-")
           << '\n';


        os << "Owner: " << GetOwner(p.string()) << std::endl;

        std::string sum_md5 = DoCKSUM(p.string(), "md5");
        std::string sum_sha1 = DoCKSUM(p.string(), "sha1");
        std::string sum_ck = DoCKSUM(p.string(), "cksum");

        os << "md5sum: " << sum_md5 << std::endl;
        os << "sha1sum: " << sum_sha1 << std::endl;
        os << "cksum: " << sum_ck << std::endl;

    }
}

size_t number_of_files_in_directory(std::filesystem::path path) {
    using std::filesystem::directory_iterator;
    return std::distance(directory_iterator(path), directory_iterator{});
}

bool CheckExistFile(fs::path path, std::string file) {
    std::ifstream fin(path.string());
    while (!fin.eof()) {
        std::string str;
        std::getline(fin, str);
        if (str.find(file) != -1) {
            return true;
        }
    }
    return false;
}

bool CheckCKSUM(const std::string &p, const std::string& var_sum) {
    std::ifstream fin(p);
    int count = 0;
    while (!fin.eof()) {
        std::string str;
        std::getline(fin, str);
        if (str.empty()) {
            break;
        }
        if (str.find(' ') != -1 && str.find('.') != -1) {
            int a;
            ++count;
            a = str.find(' ');
            std::string file = str.substr(a + 1, str.length() - a);
            str.erase(a, str.length() - a);
            if (DoCKSUM(file, var_sum) != str) {
                std::cout << "Error in " << file << std::endl;
                return false;
            }
        }

    }
    fs::path path_(p);
    int a = number_of_files_in_directory(path_.parent_path());
    if (a != count + 1) { //+1 for cksum.ini
        std::ofstream fout(p, std::ios::app);
        fout << "NEW FILES:" << std::endl;
        for (auto it: fs::recursive_directory_iterator(path_.parent_path())) {
            if (it.path().string() != p && !CheckExistFile(path_, it.path().string())) {
                std::string cksum = DoCKSUM(it.path().string(), var_sum);
                fout << cksum << " " << it.path().string() << std::endl;
            }
        }
    }


    return true;
}


int main(int argc, char *argv[]) {

    po::options_description desc("Options:");

    desc.add_options()("help", ": HELPER")
            ("filename", po::value<std::string>(), ": IS NAME OF FILE")
            ("to_file", po::value<std::string>(), ": IS NAME OF FILE TO PRINT")
            ("-c", "IS NAME OF FILE OF CKSUM")
            ("from_file", po::value<std::string>(), ": IS NAME OF FILE TO CHECK IN")
            ("var_sum", po::value<std::string>(), " IS NAME OF CKSUM");


    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }
    std::string var_sum;
    if(vm.count("var_sum")){
        var_sum = vm["var_sum"].as<std::string>();
    }
    std::string to_file;
    if (vm.count("to_file")) {
        to_file = vm["to_file"].as<std::string>();
    }
    std::string from_file;
    if (vm.count("from_file")) {
        from_file = vm["from_file"].as<std::string>();
    }

    std::string filename;
    bool ck = false;
    if (vm.count("filename")) {
        filename = vm["filename"].as<std::string>();
        if (vm.count("-c")) {
            ck = true;
        }
    } else {
        filename = "../files/cksum.ini";
    }

    if (argc == 1) {
        fs::path p("../");

        for (auto it: fs::recursive_directory_iterator(p)) {
            if (it.path().filename() == "cksum.ini") {
                bool a = CheckCKSUM(it.path().string(), var_sum);
                if (a == 1) {
                    std::cout << std::endl;
                    std::cout << "Its ok!" << std::endl;
                } else {
                    std::cout << std::endl;
                    std::cout << "Not correct CKsum" << std::endl;
                }
            }
        }
    }
    if (argc > 1 && vm.count("filename")) {
        fs::path p(filename);
        if (!ck) {
            if (is_directory(p)) {
                for (auto it: fs::recursive_directory_iterator(p)) {
                    if (!vm.count("to_file")) {
                        Print(it.path(), std::cout);
                    } else {
                        std::ofstream fout(to_file);
                        Print(it.path(), fout);
                        fout.close();
                    }
                }
            } else {
                if (!vm.count("to_file")) {
                    Print(p, std::cout);
                } else {
                    std::ofstream fout(to_file);
                    Print(p, fout);
                    fout.close();
                }
            }
        } else {
            bool a = CheckCKSUM(filename, var_sum);
            if (a == 1) {
                std::cout << "Its ok!";
            } else {
                std::cout << "Not correct CKsum";
            }
        }
    } else if (argc > 1 && vm.count("from_file")) {
        std::ifstream fin(from_file);
        while (!fin.eof()) {
            std::string str;
            std::getline(fin, str);
            if (!str.empty()) {
                fs::path p(str);
                if (is_directory(p)) {
                    for (auto it: fs::recursive_directory_iterator(p)) {
                        if (!vm.count("to_file")) {
                            Print(it.path(), std::cout);
                        } else {
                            std::ofstream fout(to_file, std::ios::app);
                            Print(it.path(), fout);
                            fout.close();
                        }
                    }
                } else {
                    if (!vm.count("to_file")) {
                        Print(p, std::cout);
                    } else {
                        std::ofstream fout(to_file, std::ios::app);
                        Print(p, fout);
                        fout.close();
                    }
                }
            }
        }
    }
}
