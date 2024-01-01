#include <iostream>
int execute(const char* command) {
	std::cout << command << std::endl;
	return system(command);
}
int repositoryAddPakage(char* pkgBuildPath,char* pakageUrl) {
	mkdtemp("/tmp/spm-devXXXXXX");
	execute("cd /srv/spm-dev");
}
int repositoryAddPakage(char* pkgBuildPath, char* pakagePath) {

}
