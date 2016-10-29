#include "usage.h"

signed
print_usage (enum pb_cmd command, signed status) {

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    FILE * f = status ? stderr : stdout;
    #pragma clang diagnostic pop

    const char * help;
    const char * op;

    switch ( command ) {
        case SNC: op = "{-S --sync}";     help = sync_help; break;
        case RMV: op = "{-R --remove}";   help = rem_help;  break;
        case UPD: op = "{-U --update}";   help = upd_help;  break;
        case DBS: op = "{-D --database}"; help = dbs_help;  break;
        case SHR:
        case NON: op = "<operation>";     help = cmds_help; break;
    }

    fprintf(f, "%s: pbpst %s [option ...]\n", _("Usage"), op);

    status = fprintf(f, "%s%s", help, gen_help);

    if ( command == NON ) {
        status += fprintf(f, "%s", more_info);
    }

    return status;
}
