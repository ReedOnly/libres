from tests import ResTest
from res.test import ErtTestContext

from res.enkf import ESUpdate


class ESUpdateTest(ResTest):

    def test_create(self):
        config = self.createTestPath("local/custom_kw/mini_config")
        with ErtTestContext("python/enkf/data/custom_kw_simulated", config) as context:
            ert = context.getErt()
            es_update = ESUpdate( ert )

            self.assertFalse( es_update.hasModule( "NO_NOT_THIS_MODULE" ))
            with self.assertRaises(KeyError):
                m = es_update.getModule( "STD_ENKF_XXX" )

            module = es_update.getModule( "STD_ENKF" )


