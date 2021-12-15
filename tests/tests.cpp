#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(GTEST_CONCAT_TOKEN_(BACKEND_NAME, _special), help){
	const char *argv[]{"--help"};
	CLIRawArgs const args{.argv0=testProgName, .argvRest = argv};

	SArg<ArgType::flag> odfA{'a', "odf", "opt dashed flag", 0, "flag", "units", false};
	SArg<ArgType::s4> odiA{'b', "odi", "opt dashed int", 0, "int", "units", 0};
	SArg<ArgType::string> odsA{'c', "ods", "opt dashed str", 0, "string", "units", ""};

	SArg<ArgType::flag> mdfA{'d', "mdf", "mand dashed flag", 1, "flag", "units", false};
	SArg<ArgType::s4> mdiA{'e', "mdi", "mand dashed int", 1, "int", "units", 0};
	SArg<ArgType::string> mdsA{'f', "mds", "mand dashed str", 1, "string", "units", ""};

	SArg<ArgType::flag> mpfA{'g', "mpf", "mand pos flag", 1, "flag", "units", false};
	SArg<ArgType::s4> mpiA{'i', "mpi", "mand pos int", 1, "int", "units", 0};
	SArg<ArgType::string> mpsA{'j', "mps", "mand pos str", 1, "string", "units", ""};

	SArg<ArgType::flag> opfA{'k', "opf", "opt pos flag", 0, "flag", "units", false};
	SArg<ArgType::s4> opiA{'l', "opi", "opt pos int", 0, "int", "units", 0};
	SArg<ArgType::string> opsA{'m', "ops", "opt pos str", 0, "string", "units", ""};

	const std::vector<Arg*> dashedSpec{
		&odfA, &odiA, &odsA,
		&mdfA, &mdiA, &mdsA
	};

	const std::vector<Arg*> positionalSpec{
		&mpfA, &mpiA, &mpsA,
		&opfA, &opiA, &opsA,
	};

	std::unique_ptr<IArgsParser> ap{argsParserFactory(testProgName, testProgramDescription, testProgramFallbackUsage, dashedSpec, positionalSpec)};
	auto res = (*ap)(args);
	ASSERT_EQ(res.parsingStatus.returnCode, EXIT_SUCCESS);

	ASSERT_TRUE(res.parsingStatus.events.helpCalled);
	ASSERT_FALSE(res.parsingStatus.events.autoCompleteCalled);
	ASSERT_FALSE(res.parsingStatus.events.syntaxError);
}
