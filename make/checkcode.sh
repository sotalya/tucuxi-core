clang-tidy \
	-config="{Checks: '-*,readability-identifier-naming', \
		CheckOptions: [ \
			{ key: readability-identifier-naming.ClassCase,           value: CamelCase }, \
			{ key: readability-identifier-naming.MemberPrefix,        value: m_        }, \
			{ key: readability-identifier-naming.VariableCase,        value: camelBack }, \
			{ key: readability-identifier-naming.ParameterCase,       value: camelBack }, \
			{ key: readability-identifier-naming.ParameterPrefix,     value: '_'       }, \
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
	-std=c++14
