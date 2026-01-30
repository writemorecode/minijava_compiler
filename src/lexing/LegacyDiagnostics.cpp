#include "lexing/LegacyDiagnostics.hpp"

#include <iostream>

namespace lexing {

LegacyDiagnosticSink::LegacyDiagnosticSink(int *error_flag, std::ostream &out)
    : error_flag_(error_flag), out_(&out) {}

void LegacyDiagnosticSink::emit(Diagnostic d) {
    if (d.severity == Severity::Error) {
        error_count_ += 1;
        if (error_flag_ != nullptr) {
            *error_flag_ = 1;
        }
    }

    if (out_ != nullptr) {
        (*out_) << d.message;
    }
}

} // namespace lexing
