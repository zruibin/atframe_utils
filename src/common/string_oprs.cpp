#include <common/string_oprs.h>
#include <sstream>


namespace util {
    namespace string {
        const char *version_tok(const char *v, int64_t &out) {
            if (NULL == v) {
                out = 0;
                return v;
            }

            while (*v && (*v == ' ' || *v == '\t' || *v == '\r' || *v == '\n')) {
                ++v;
            }

            if (*v) {
                out = to_int<int64_t>(v);
            } else {
                out = 0;
            }

            // skip digital
            while (*v && *v != '.' && *v != '-') {
                ++v;
            }

            // skip dot or minus
            if (*v) {
                ++v;
            }

            return v;
        }

        int version_compare(const char *l, const char *r) {
            while ((l && *l) || (r && *r)) {
                int64_t lver = 0;
                int64_t rver = 0;

                l = version_tok(l, lver);
                r = version_tok(r, rver);

                if (lver != rver) {
                    return lver < rver ? -1 : 1;
                }
            }

            return 0;
        }

        std::string version_normalize(const char *v) {
            std::stringstream ss;

            bool need_dot = false;
            while (v && *v) {
                if (need_dot) {
                    ss << '.';
                } else {
                    need_dot = true;
                }

                int64_t n = 0;

                v = version_tok(v, n);
                ss << n;
            }

            std::string ret = ss.str();
            while (ret.size() > 2 && ret[ret.size() - 2] == '.' && ret[ret.size() - 1] == '0') {
                ret.pop_back();
                ret.pop_back();
            }

            if (ret.empty()) {
                ret = "0";
            }

            return ret;
        }

    } // namespace string
} // namespace util