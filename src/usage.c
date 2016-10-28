#include "usage.h"

signed
print_usage (enum pb_cmd command, signed status) {


    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    FILE * f = status ? stderr : stdout;
    #pragma clang diagnostic pop

    const char * help;

    switch ( command ) {
        case SNC: help = sync_help; break;
        case RMV: help = rem_help;  break;
        case UPD: help = upd_help;  break;
        case DBS: help = dbs_help;  break;
        case SHR:
        case NON: help = cmds_help; break;
    }

    status = fprintf(f, "%s%s", help, gen_help);

    if ( command == NON ) {
        status += fprintf(f, "%s", more_info);
    }

    return status;
}
