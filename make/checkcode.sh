clang-tidy \
	-config="{Checks: '-*,readability-braces-around-statements, \
                              readability-identifier-naming, \
                              readability-avoid-const-params-in-decls, \
                              clang-analyzer-*, \
                              bugprone-integer-division, \
                              google-readability-casting, \
                              llvm-namespace-comment, \
                              google-global-names-in-headers, \
                              misc-unused-parameters, \
                              modernize-loop-convert, \
                              modernize-make-unique, \
                              modernize-use-nullptr, \
                              modernize-use-override, \
                              modernize-raw-string-literal, \
                              readability-implicit-bool-conversion, \
                              cppcoreguidelines-narrowing-conversions, \
                              bugprone-multiple-statement-macro, \
                              bugprone-integer-division, \
                              readability-inconsistent-declaration-parameter-name,bugprone-*,cppcoreguidelines-avoid-goto,cppcoreguidelines-avoid-non-const-global-variables,cppcoreguidelines-avoid-c-arrays,cppcoreguidelines-c-copy-assignment-signature,cppcoreguidelines-interfaces-global-init,cppcoreguidelines-macro-usage,cppcoreguidelines-narrowing-conversions,cppcoreguidelines-no-malloc,cppcoreguidelines-pro-*,cppcoreguidelines-slicing,performance-*,readability-avoid-const-params-in-decls,readability-const-return-type,readability-container-size-empty,readability-delete-null-pointer,readability-deleted-default,readability-else-after-return,readability-function-size,readability-identifier-naming,readability-inconsistent-declaration-parameter-name,readability-isolate-declaration,readability-misleading-indentation,readability-misplaced-array-index,readability-non-const-parameter,readability-redundant-*,readability-simplify-*,readability-static-*,readability-string-compare', \
		CheckOptions: [ \
			{ key: readability-identifier-naming.ClassCase,           value: CamelCase }, \
			{ key: readability-identifier-naming.MemberPrefix,        value: m_        }, \
			{ key: readability-identifier-naming.VariableCase,        value: camelBack }, \
			{ key: readability-identifier-naming.ParameterCase,       value: camelBack }, \
			{ key: readability-identifier-naming.ParameterPrefix,     value: '_'       }, \
                        { key: readability-identifier-naming.EnumCase,            value: CamelCase }, \
                        { key: readability-identifier-naming.EnumConstCase,       value: CamelCase }, \
                        { key: readability-identifier-naming.StaticConstantCase,  value: UPPER_CASE }, \
                        { key: readability-identifier-naming.GlobalConstantCase,  value: UPPER_CASE }, \



			
		]}" \
	-header-filter="../src/tucucore/|../src/tucucrypto/|../src/tucudrugfilechecker/|../src/tucucommon/|../src/tucucli/|../src/tuculicense/../src/tucuquery/../src/tucuvalidator/" \
	../src/tucucli/*.cpp \
	../src/tucucommon/*.cpp \
	../src/tucucrypto/*.cpp \
	../src/tucucore/*.cpp \
	../src/tucudrugfilechecker/*.cpp \
	../src/tuculicense/*.cpp \
	../src/tucuquery/*.cpp \
	../src/tucuvalidator/*.cpp \
	-- \
	-I../src \
	-I../libs/cxxopts \
	-I../libs/rapidjson-master-20190220/include \
	-I../libs/eigen-3.3.2 \
	-I../libs/boost-1.63.0 \
	-I../libs/date/include \
	-I../libs/spdlog-master-20170622/include \
    -I../libs/botan-2.1.0/build/include \
    -I../libs/tiny-js-master-20170629 \
    -I../libs/rapidxml-1.13 \
	-std=c++14 \
> clang-warnings.txt


# Other checks:
#                              readability-magic-numbers,
# cppcoreguidelines-special-member-functions

# I don't know why I cannot get the static variables correctly analyzed
# OK, I think because it applies to static variables, not class static variables
                       # { key: readability-identifier-naming.StaticVariablePrefix,value: sm_       }, \


grep "\.h" clang-warnings.txt | sort -u > clang-warnings-shorts-h.txt
grep "\.cpp" clang-warnings.txt | sort -u > clang-warnings-shorts-cpp.txt



# The following, if used instead of -header-filter, will only display warnings for these two files
#    -line-filter="[{\"name\" : \"../src/tucucore/deriv.h\"},{\"name\" : \"../src/tucucore/minimize.h\"}]" \

