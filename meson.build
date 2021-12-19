project('gamingtool', 'c')

jsondep = dependency('cJSON')

executable('out', 
['main.c', 'parse.c', 'filesys.c', 'errors.c', 'run.c', 'list.c', 'info.c', 'utils.c', 'config.c'],
  dependencies: [jsondep]
)
