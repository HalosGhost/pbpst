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
        case SNC: op = "-S --sync";     help = sync_help; break;
        case RMV: op = "-R --remove";   help = rem_help;  break;
        case UPD: op = "-U --update";   help = upd_help;  break;
        case DBS: op = "-D --database"; help = dbs_help;  break;
        case SHR:
        case NON: op = _("operation");  help = cmds_help; break;
    }

    status =
        fprintf(f, "%s: pbpst {%s} [%s ...]\n", _("Usage"), op, _("option"));

    if ( !command ) {
        status += fprintf(f, "pbpst -- %s\n",
                          _("a simple tool to pastebin from the command-line"));
    }

    status = fprintf(f, "\n%s:\n%s\n",
                     command ? _("Options") : _("Operations"), help);

    if ( !command ) { status += fprintf(f, "%s:\n", _("Options")); }

    status += fprintf(f, "%s", gen_help);

    if ( !command ) {
        status += fprintf(f, "\n%s\n",
                          _("Use `-h` with an operation for more help"));
    } return status;
}
