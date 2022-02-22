project('gamingtool', 'c', default_options: ['optimization=3'])

jsondep = dependency('cJSON')

executable('out', 
['main.c', 'parse.c', 'filesys.c', 'errors.c', 'run.c', 'list.c', 'info.c', 'str.c', 'utils.c', 'config.c', 'create.c'],
  dependencies: [jsondep]
)
