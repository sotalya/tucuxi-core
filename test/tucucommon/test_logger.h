
#include "fructose/fructose.h"
#include "tucucommon/logger.h"

class some_class {};
std::ostream& operator<<(std::ostream& os, const some_class& c)
{ 
    return os << "some_class"; 
}

struct TestLogger : public fructose::test_base<TestLogger> 
{
	void basic(const std::string& _testName)
	{
		Logger logger;
		logger.debug("Tcho les topiots");
		logger.info("Tcho les {}", "topiots");
		logger.warn("{} + {} = {}", 2, 2, 4);
		logger.error("Tcho les topiots");
		logger.critical("Tcho les topiots");

		// Todo: Check content of log file...
	}

	void logMyClass(const std::string& _testName)
	{
		Logger logger;
 		some_class c;
 		logger.info("Logging a custom class with operator<<: {}", c);

 		// Todo: Check content of log file...
	}
 };

 

