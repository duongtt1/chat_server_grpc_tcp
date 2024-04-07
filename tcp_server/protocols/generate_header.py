from jinja2 import Environment, FileSystemLoader
import xml.etree.ElementTree as ET

# Load the XML protocol
tree = ET.parse('protocol.xml')
root = tree.getroot()

# Extract data types
libs = []
files = []
namespaces = []

for lib in root.findall('./options/libs/lib'):
    libs.append(lib.text)

for f in root.findall('./options/files/file'):
    files.append(f.text)

for n in root.findall('./options/namespaces/namespace'):
    namespaces.append(n.text)
    
enums = []

for enum in root.findall('./datatypes/usertype/enums/enum'):
    enum_name = enum.attrib['name']
    enum_type = enum.attrib['type']
    enum_elements = [(element.attrib['name'], element.attrib['desc']) for element in enum.findall('element')]
    enums.append((enum_name, enum_type, enum_elements))


# Extract request details
requests = []

for req in root.findall('./behaviors/requests/req'):
    req_name = req.attrib['name']
    req_id = req.attrib['id']
    req_opcode = req.attrib['opcode']
    params = [(param.attrib['name'], param.attrib['type']) for param in req.findall('params/param')]
    returns = [(param.attrib['name'], param.attrib['type']) for param in req.findall('returns/param')]
    requests.append((req_name, req_id, req_opcode, params, returns))


# Render the header file using Jinja template
env = Environment(loader=FileSystemLoader('.'))

template = env.get_template('./templates/request_template.jinja')
template_request_handler = env.get_template('./templates/request_handler.jinja')
types = env.get_template('./templates/types.jinja')
factory = env.get_template('./templates/factory_template.jinja')
response = env.get_template('./templates/response_template.jinja')

rendered_template = template.render(includes=libs,
    enums=enums, 
    files=files,
    requests=requests,
    namespaces=namespaces)

rendered_req_handler_template = template_request_handler.render(includes=libs,
    enums=enums, 
    files=files,
    requests=requests,
    namespaces=namespaces)

type_template = types.render(includes=libs,
    enums=enums, 
    files=files,
    requests=requests,
    namespaces=namespaces)

factory_template = factory.render(includes=libs,
    enums=enums, 
    files=files,
    requests=requests,
    namespaces=namespaces)

response_template = response.render(includes=libs,
    enums=enums, 
    files=files,
    requests=requests,
    namespaces=namespaces)

# Write the rendered template to a file
with open('./generate/request_template.h', 'w') as f:
    f.write(rendered_template)

# Write the rendered template to a file
with open('./generate/request_handler.h', 'w') as f:
    f.write(rendered_req_handler_template)

with open('./generate/types.h', 'w') as f:
    f.write(type_template)

# Write the rendered template to a file
with open('./generate/factory_request.h', 'w') as f:
    f.write(factory_template)

with open('./generate/response_template.h', 'w') as f:
    f.write(response_template)
