#include "../include/gpg.h"
#include "../include/cli.h"

int main(int argc, char *argv[]) {
    init();

    if (verify_args(argc, argv) != 0) usage();
    
    return 0;
}
