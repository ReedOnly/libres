from ert.enkf import EnkfStateType, RealizationStateEnum, EnkfNode, ErtImplType, NodeId
from ert.enkf.plot_data import PlotBlockData, PlotBlockVector
from ert.util import DoubleVector, BoolVector, ThreadPool


class PlotBlockDataLoader(object):

    def __init__(self, obs_vector):
        """
        @type obs_vector: ObsVector
        """
        super(PlotBlockDataLoader, self).__init__()
        self.__obs_vector = obs_vector
        self.__permutation_vector = None


    def getDepthValues(self, report_step):
        """ @rtype: DoubleVector """
        block_obs = self.__obs_vector.getNode(report_step)
        """:type: BlockObservation """

        depth = DoubleVector()
        for index in block_obs:
            value = block_obs.getDepth(index)
            depth.append(value)

        return depth


    def load(self, fs, report_step, state=EnkfStateType.FORECAST, input_mask=None):
        """
         @type fs: EnkfFs
         @type report_step: int
         @type state: EnkfStateType
         @type input_mask: BoolVector
         @rtype: PlotBlockData
        """

        state_map = fs.getStateMap()
        ensemble_size = len(state_map)

        if not input_mask is None:
            mask = BoolVector.copy(input_mask)
        else:
            mask = BoolVector(False, ensemble_size)

        state_map.selectMatching(mask, RealizationStateEnum.STATE_HAS_DATA)

        depth = self.getDepthValues(report_step)

        self.__permutation_vector = depth.permutationSort()
        depth.permute(self.__permutation_vector)

        plot_block_data = PlotBlockData(depth)

        thread_pool = ThreadPool()
        for index in range(ensemble_size):
            if mask[index]:
                thread_pool.addTask(self.loadVector, plot_block_data, fs, report_step, index, state)

        thread_pool.nonBlockingStart()
        thread_pool.join()

        return plot_block_data


    def loadVector(self, plot_block_data, fs, report_step, realization_number, state=EnkfStateType.FORECAST):
        """
        @type plot_block_data: PlotBlockData
        @type fs: EnkfFs
        @type report_step: int
        @type realization_number: int
        @type state: EnkfStateType
        @rtype PlotBlockVector
        """
        config_node = self.__obs_vector.getConfigNode()

        is_private_container = config_node.getImplementationType() == ErtImplType.CONTAINER
        data_node = EnkfNode(config_node, private=is_private_container)

        node_id = NodeId(report_step, realization_number, state)

        if data_node.tryLoad(fs, node_id):
            block_obs = self.__obs_vector.getNode(report_step)
            """:type: BlockObservation """

            data = DoubleVector()
            for index in range(len(block_obs)):
                value = block_obs.getData(data_node.valuePointer(), index, node_id)
                data.append(value)
            data.permute(self.__permutation_vector)

            plot_block_vector = PlotBlockVector(realization_number, data)
            plot_block_data.addPlotBlockVector(plot_block_vector)
