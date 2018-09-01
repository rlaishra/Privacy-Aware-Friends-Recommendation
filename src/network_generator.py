import networkx as nx 
import random, math, csv

class NetworkGenerator(object):
	"""Generate sythetic multi-layer social network"""
	def __init__(self, layer_count, layer_size, profile_length,\
	 layer_connectivity, prefix):
		super(NetworkGenerator, self).__init__()
		self.__layer_count = layer_count
		self.__layer_size = layer_size
		self.__layer_connectivity = layer_connectivity
		self.__prefix = prefix
		self.__graphs = {}
		self.__nodes = {}
		self.__inter_layer = {}
		self.__composed_graph = None
		self.__profile_length = profile_length
		self.__profile = {}


	def __generate_layer(self, layer_name):
		graph = nx.barabasi_albert_graph(self.__layer_size, 5)
		mapping = {}
		self.__nodes[layer_name] = []
		for x in xrange(0, self.__layer_size):
			mapping[x] = str(layer_name) + '_' + str(x)
			self.__nodes[layer_name].append(mapping[x])

		graph = nx.relabel_nodes(graph, mapping)

		return graph


	def __generate_inter_layer(self):
		for x in xrange(0, self.__layer_count):
			other_layers = [y for y in xrange(0, self.__layer_count) if y != x]
			print(x, other_layers)
			for z in xrange(0, int(self.__layer_connectivity *\
			 self.__layer_size)):
				source_node = random.choice(self.__nodes[x])
				dest_layer = random.choice(other_layers)
				dest_node = random.choice(self.__nodes[dest_layer])
				print(x, source_node, dest_layer, dest_node)
				if source_node in self.__inter_layer:
					self.__inter_layer[source_node].append(dest_node)
				else:
					self.__inter_layer[source_node] = [dest_node]
				if dest_node in self.__inter_layer:
					self.__inter_layer[dest_node].append(source_node)
				else:
					self.__inter_layer[dest_node] = [source_node]

	def __generate_composed_graphs(self):
		self.__composed_graph = self.__graphs[0]
		for x in xrange(1,self.__layer_count):
			self.__composed_graph = nx.compose(self.__composed_graph,\
			 self.__graphs[x])

		for x in self.__inter_layer:
			for y in self.__inter_layer[x]:
				self.__composed_graph.add_edge(x,y)


	def __generate_profile(self):
		for x in self.__nodes:
			self.__profile[x] = {}
			for y in self.__nodes[x]:
				self.__profile[x][y] = random.sample([0,1]*self.__profile_length\
				 , self.__profile_length)

	def __save_profile(self):
		with open('data/'+self.__prefix+'_composed_profile.csv', 'wb') as fc:
			cwriter = csv.writer(fc, delimiter=' ',quotechar='|', \
				quoting=csv.QUOTE_MINIMAL)
			for layer in self.__profile:
				with open('data/'+self.__prefix+'_'+str(layer)+'_profile.csv', 'wb') as fl:
					lwriter = csv.writer(fl, delimiter=' ',quotechar='|', \
					 quoting=csv.QUOTE_MINIMAL)
					for x in self.__profile[layer]:
						cwriter.writerow([x] + self.__profile[layer][x])
						lwriter.writerow([x] + self.__profile[layer][x])

	def __save_composed(self):
		alist = nx.generate_adjlist(self.__composed_graph)
		with open('data/'+self.__prefix+'_composed_graph.csv', 'wb') as f:
			writer = csv.writer(f, delimiter=' ',quotechar='|', \
				quoting=csv.QUOTE_MINIMAL)
			for x in alist:
				writer.writerow(x.split())

	
	def __save_inter_layer(self):
		with open('data/'+self.__prefix+'_inter_layer.csv', 'wb') as f:
			writer = csv.writer(f, delimiter=' ',quotechar='|', \
				quoting=csv.QUOTE_MINIMAL)
			for x in self.__inter_layer:
				writer.writerow([x] + self.__inter_layer[x])


	def __save_layer(self, graph, layer):
		alist = nx.generate_adjlist(graph)

		with open('data/'+self.__prefix+'_'+str(layer)+'_graph.csv', 'wb') as f:
			writer = csv.writer(f, delimiter=' ',quotechar='|', \
				quoting=csv.QUOTE_MINIMAL)
			for x in alist:
				writer.writerow(x.split())


	def run(self):
		for x in xrange(0, self.__layer_count):
			self.__graphs[x] = self.__generate_layer(x)
			self.__save_layer(self.__graphs[x], x)
		
		self.__generate_inter_layer()
		self.__save_inter_layer()

		self.__generate_composed_graphs()
		self.__save_composed()

		self.__generate_profile()
		self.__save_profile()
		
