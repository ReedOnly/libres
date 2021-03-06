set(TEST_SOURCES
    __init__.py
    test_ert_plugin.py
    test_ert_script.py
    test_ext_job.py
    test_function_ert_script.py
    test_job_queue.py
    test_forward_model_formatted_print.py
    test_workflow.py
    test_workflow_job.py
    test_workflow_joblist.py
    test_workflow_runner.py
    test_jobmanager.py
    test_job_manager_runtime_kw.py
    test_statoil_jobmanager.py
    workflow_common.py
)

add_python_package("python.tests.res.job_queue" ${PYTHON_INSTALL_PREFIX}/tests/res/job_queue "${TEST_SOURCES}" False)

addPythonTest(tests.res.job_queue.test_forward_model_formatted_print.ForwardModelFormattedPrintTest)
addPythonTest(tests.res.job_queue.test_job_queue.JobQueueTest)
addPythonTest(tests.res.job_queue.test_workflow_job.WorkflowJobTest)
addPythonTest(tests.res.job_queue.test_workflow.WorkflowTest)
addPythonTest(tests.res.job_queue.test_workflow_joblist.WorkflowJoblistTest)
addPythonTest(tests.res.job_queue.test_ert_plugin.ErtPluginTest)
addPythonTest(tests.res.job_queue.test_ert_script.ErtScriptTest)
addPythonTest(tests.res.job_queue.test_function_ert_script.FunctionErtScriptTest)
addPythonTest(tests.res.job_queue.test_workflow_runner.WorkflowRunnerTest)
addPythonTest(tests.res.job_queue.test_ext_job.ExtJobTest)
addPythonTest(tests.res.job_queue.test_jobmanager.JobManagerTest)
addPythonTest(tests.res.job_queue.test_job_manager_runtime_kw.JobManagerTestRuntimeKW)

if (STATOIL_TESTDATA_ROOT)
    addPythonTest(tests.res.job_queue.test_statoil_jobmanager.JobManagerStatoilTest
                  LABELS StatoilData)
endif()
