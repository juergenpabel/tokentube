import sys, importlib, pkgutil


def init(parser):
	modules = loadAllModules()
	for module in modules.values():
		module.init(parser)


def loadAllModules():
	package = sys.modules[__name__]
	return {
		name: importlib.import_module(__name__ + '.' + name)
		for loader, name, is_pkg in pkgutil.walk_packages(package.__path__)
	}

