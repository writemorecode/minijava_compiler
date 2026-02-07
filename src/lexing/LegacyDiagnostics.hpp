#pragma once

#include <iostream>
#include <ostream>

#include "lexing/Diagnostics.hpp"

namespace lexing {

class LegacyDiagnosticSink final : public DiagnosticSink {
  public:
    explicit LegacyDiagnosticSink(int *error_flag = nullptr,
                                  std::ostream &out = std::cerr);

    void emit(Diagnostic d) override;

    int error_count() const { return error_count_; }

  private:
    int error_count_ = 0;
    int *error_flag_ = nullptr;
    std::ostream *out_ = nullptr;
};

} // namespace lexing
