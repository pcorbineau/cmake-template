# Called by the coverage target to run llvm-cov export and redirect output to a file.
# Arguments passed via -D:
#   LLVM_COV      - path to llvm-cov executable
#   TARGET_FILE   - path to the instrumented test binary
#   PROFDATA      - path to the .profdata file
#   IGNORE_REGEX  - filename regex to ignore
#   OUTPUT        - path to write the JSON summary

execute_process(
    COMMAND
        ${LLVM_COV} export -object ${TARGET_FILE} -instr-profile=${PROFDATA}
        -ignore-filename-regex=${IGNORE_REGEX} -format=text -summary-only
    OUTPUT_FILE ${OUTPUT}
    COMMAND_ERROR_IS_FATAL ANY
)
