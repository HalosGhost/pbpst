#include "usage.h"

signed
print_usage (const enum pb_cmd command, signed status) {

    const char * op;

    switch ( command ) {
        case SNC: op = "-S --sync";     break;
        case RMV: op = "-R --remove";   break;
        case UPD: op = "-U --update";   break;
        case DBS: op = "-D --database"; break;
        case SHR:
        case NON: op = _("operation");  break;
    }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    FILE * f = status ? stderr : stdout;
    #pragma clang diagnostic pop

    status = fprintf(f, "%s: pbpst <%s> [%s ...]\npbpst -- %s\n\n%s:\n",
                     _("Usage"), op, _("option"),
                     _("a simple tool to pastebin from the command-line"),
                     !command ? _("Operations") : _("Options"));

    status += enum_args(command, f);
    if ( !command ) {
        status += fprintf(f, "\n%s:\n", _("Options"));
    } else {
        status += fputc('\n', f);
    } status += enum_args(SHR, f);

    if ( !command ) {
        status += fprintf(f, "\n%s\n",
                          _("Use `-h` with an operation for more help"));
    } return status;
}

signed
enum_args (enum pb_cmd cmd, FILE * f) {

    #define X(sh, ln, arg, desc) { ln, arg, desc, sh },
    struct optdesc SNC_opts [] = { SNC_OPTIONS },
                   RMV_opts [] = { RMV_OPTIONS },
                   UPD_opts [] = { UPD_OPTIONS },
                   DBS_opts [] = { DBS_OPTIONS },
                   GEN_opts [] = { GEN_OPTIONS },
                   NON_opts [] = { OPERATIONS  };
    #undef X

    #define optarr(x) (x ## _opts)
    #define optlen(x) (sizeof (optarr(x)) / sizeof (struct optdesc))

    signed status = 0;
    char longbuf [16] = "";
    size_t len = 0;
    struct optdesc * opts = 0;
    switch ( cmd ) {
        case SNC: len = optlen(SNC); opts = optarr(SNC); break;
        case UPD: len = optlen(UPD); opts = optarr(UPD); break;
        case RMV: len = optlen(RMV); opts = optarr(RMV); break;
        case DBS: len = optlen(DBS); opts = optarr(DBS); break;
        case SHR: len = optlen(GEN); opts = optarr(GEN); break;
        case NON: len = optlen(NON); opts = optarr(NON); break;
    }

    for ( size_t i = 0 ; i < len; ++ i ) {
        if ( !opts[i].flag ) { break; }
        if ( opts[i].flag < 256 ) {
            status += fprintf(f, "  -%c, ", opts[i].flag);
        } else {
            status += fputs("      ", f);
        }
        snprintf(longbuf, 16, "--%s%c%s",
                opts[i].full,
                opts[i].arg ? '=' : ' ',
                opts[i].arg ? opts[i].arg : "");

        status += fprintf(f, "%-15s  %s\n", longbuf, opts[i].desc);

        // workarounds for old API mistakes
        if ( cmd == UPD ) {
            len = optlen(SNC) - 1; opts = optarr(SNC); i = -1ULL; cmd = NON;
        } else if ( cmd == RMV ) {
            len = optlen(UPD); opts = optarr(UPD); i = -1ULL; cmd = NON;
        } else if ( cmd == DBS && i == len - 1 ) {
            len = optlen(RMV); opts = optarr(RMV); i = -1ULL; cmd = NON;
        }
    } return status;
}
