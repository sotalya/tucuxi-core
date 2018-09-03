clang-tidy \
	-config="{Checks: '-*,readability-braces-around-statements, \
                              readability-identifier-naming, \
                              readability-avoid-const-params-in-decls, \
                              google-readability-casting, \
                              llvm-namespace-comment, \
                              google-global-names-in-headers, \
                              misc-unused-parameters,
                              modernize-make-unique,
                              modernize-use-nullptr,
                              modernize-use-override,
                              modernize-raw-string-literal,
                              readability-implicit-bool-conversion,
                              cppcoreguidelines-narrowing-conversions,
                              readability-magic-numbers,
                              bugprone-multiple-statement-macro,
                              bugprone-integer-division,
                              readability-inconsistent-declaration-parameter-name', \
		CheckOptions: [ \
			{ key: readability-identifier-naming.ClassCase,           value: CamelCase }, \
			{ key: readability-identifier-naming.MemberPrefix,        value: m_        }, \
			{ key: readability-identifier-naming.VariableCase,        value: camelBack }, \
			{ key: readability-identifier-naming.ParameterCase,       value: camelBack }, \
			{ key: readability-identifier-naming.ParameterPrefix,     value: '_'       }, \
                        { key: readability-identifier-naming.StaticVariableCase,  value: camelBack }, \
                        { key: readability-identifier-naming.EnumCase,            value: CamelCase }, \
		]}" \
	-header-filter="../src/tucucore/|../src/tucucommon/|../src/tucucli/|../src/tuculicense/" \
	../src/tucucore/*.cpp \
	../src/tucucommon/*.cpp \
	../src/tuculicense/*.cpp \
	../src/tucucli/*.cpp \
	-- \
	-I../src \
	-I../libs/eigen-3.3.2 \
	-I../libs/boost-1.63.0 \
	-I../libs/date-master-20170711 \
	-I../libs/spdlog-master-20170622/include \
    -I../libs/botan-2.1.0/build/include \
    -I../libs/tiny-js-master-20170629 \
    -I../libs/rapidxml-1.13 \
	-std=c++14 \
> clang-warnings.txt


grep "\.h" clang-warnings.txt | sort -u > clang-warnings-shorts.txt



# The following, if used instead of -header-filter, will only display warnings for these two files
#    -line-filter="[{\"name\" : \"../src/tucucore/deriv.h\"},{\"name\" : \"../src/tucucore/minimize.h\"}]" \


#                        { key: readability-identifier-naming.EnumConstantCase,    value: CamelCase }, \
