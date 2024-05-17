#define CATCH_CONFIG_RUNNER

#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/environment.hpp"
#include "dansandu/ballotin/file_system.hpp"
#include "dansandu/ballotin/logging.hpp"
#include "dansandu/ballotin/progress_bar.hpp"

using dansandu::ballotin::environment::getEnvironmentVariable;
using dansandu::ballotin::file_system::writeToStandardOutput;
using dansandu::ballotin::logging::Level;
using dansandu::ballotin::logging::Logger;
using dansandu::ballotin::logging::UnitTestsHandler;
using dansandu::ballotin::progress_bar::ProgressBar;

class ProgressBarListener : public Catch::TestEventListenerBase
{
public:
    ProgressBarListener(Catch::ReporterConfig const& _config) : TestEventListenerBase(_config)
    {
        std::set<Catch::TestCase const*> tests;
        const auto& allTestCases = getAllTestCasesSorted(*m_config);
        Catch::TestSpec::Matches matches = _config.fullConfig()->testSpec().matchesByFilter(allTestCases, *m_config);
        const auto& invalidArgs = _config.fullConfig()->testSpec().getInvalidArgs();

        if (matches.empty() && invalidArgs.empty())
        {
            for (auto const& test : allTestCases)
                if (!test.isHidden())
                    tests.emplace(&test);
        }
        else
        {
            for (auto const& match : matches)
                tests.insert(match.tests.begin(), match.tests.end());
        }

        const auto header = std::string{"test"};
        const auto headerSize = getEnvironmentVariable("PRALINE_PROGRESS_BAR_HEADER_LENGTH");

        progressBar_ = std::make_unique<ProgressBar>(
            header, headerSize.has_value() ? std::stoi(headerSize.value()) : header.size(), tests.size(),
            [](const auto& text)
            {
                std::cout << text;
                std::cout.flush();
            });
    }

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        progressBar_->updateSummary(testInfo.name);
        LOG_INFO("Starting test case '", testInfo.name, "'");
    }

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override
    {
        progressBar_->advance();
        LOG_INFO("Ending test case '", testCaseStats.testInfo.name, "'");
    }

    void testGroupEnded(Catch::TestGroupStats const& testGroupStats) override
    {
        progressBar_.reset();
    }

private:
    std::unique_ptr<ProgressBar> progressBar_;
};

CATCH_REGISTER_LISTENER(ProgressBarListener);

int main(const int argumentsCount, const char* const* const arguments)
{
    auto unitTestsHandler = UnitTestsHandler{"unit_tests.log"};

    auto& logger = Logger::globalInstance();
    logger.setLevel(Level::debug);
    logger.addHandler("UnitTests", Level::debug, unitTestsHandler);

    const auto catchResult = Catch::Session().run(argumentsCount, arguments);

    if (unitTestsHandler.errorsLogged() || unitTestsHandler.warningsLogged())
    {
        writeToStandardOutput("Tests failed: errors or warnings were logged\n");
        return -1;
    }

    return catchResult;
}
