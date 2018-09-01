addpath("octave");

%import the muxOctaveLib
muxOctaveLib;

%import the configuration file
LayersList = {};
muxOctaveConfig;

NodesTensor = {}; 
Layers = 0;
SupraAdjacencyMatrix = 0;

if isExtendedEdgesList
    [SupraAdjacencyMatrix,Layers,Nodes] = BuildSupraAdjacencyMatrixFromFile(MultiLayerEdgesListFile,Flags,MaxNodes,FirstNodeLabel);
    NodesTensor = SupraAdjacencyToNodesTensor(SupraAdjacencyMatrix,Layers,Nodes);
else
    [NodesTensor,Layers,Nodes] = BuildMultiplexFromFileList(LayersList,Flags,MaxNodes,FirstNodeLabel);         
    LayersTensor = BuildLayersTensor(Layers,Nodes,OmegaParameter,MultisliceType);
    SupraAdjacencyMatrix = BuildSupraAdjacencyMatrix(NodesTensor,LayersTensor,Layers,Nodes);
endif

if Layers > 1    
    OverlappingMatrix = GetAverageGlobalOverlappingMatrix(SupraAdjacencyMatrix,Layers,Nodes);
    outputFile = strcat(AnalysisName,"_overlapping_matrix.txt");
    dlmwrite (outputFile, OverlappingMatrix,'delimiter',' ')
    printf("Multislice overlapping output to: %s\n",outputFile);
end    
