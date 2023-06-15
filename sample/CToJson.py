

import json
import sys
import re

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])

from pycparser import parse_file, c_ast
from pycparser.plyparser import Coord


RE_CHILD_ARRAY = re.compile(r'(.*)\[(.*)\]')
RE_INTERNAL_ATTR = re.compile('__.*__')


class CJsonError(Exception):
    pass

class CToJSON:

    def child_attrs_of(self, klass):
        """
        Given a Node class, get a set of child attrs.
        Memoized to avoid highly repetitive string manipulation

        """
        non_child_attrs = set(klass.attr_names)
        all_attrs = set([i for i in klass.__slots__ if not RE_INTERNAL_ATTR.match(i)])
        return all_attrs - non_child_attrs

    def to_dict(self,node):
        """ Recursively convert an ast into dict representation. """
        klass = node.__class__

        result = {}

        # Metadata
        result['_nodetype'] = klass.__name__

        # Local node attributes
        for attr in klass.attr_names:
            result[attr] = getattr(node, attr)

        # Coord object
        if node.coord:
            result['coord'] = str(node.coord)
        else:
            result['coord'] = None

        # Child attributes
        for child_name, child in node.children():
            # Child strings are either simple (e.g. 'value') or arrays (e.g. 'block_items[1]')
            match = RE_CHILD_ARRAY.match(child_name)
            if match:
                array_name, array_index = match.groups()
                array_index = int(array_index)
                # arrays come in order, so we verify and append.
                result[array_name] = result.get(array_name, [])
                if array_index != len(result[array_name]):
                    raise CJsonError('Internal ast error. Array {} out of order. '
                                     'Expected index {}, got {}'.format(
                        array_name, len(result[array_name]), array_index))
                result[array_name].append(self.to_dict(child))
            else:
                result[child_name] = self.to_dict(child)

        # Any child attributes that were missing need "None" values in the json.
        for child_attr in self.child_attrs_of(klass):
            if child_attr not in result:
                result[child_attr] = None

        return result

    def to_json(self,node, **kwargs):
        """ Convert ast node to json string """
        return json.dumps(self.to_dict(node), **kwargs)

    def file_to_dict(self,filename):
        """ Load C file into dict representation of ast """
        ast = parse_file(filename, use_cpp=True)
        return self.to_dict(ast)

    def file_to_json(self,filename, **kwargs):
        """ Load C file into json string representation of ast """
        ast = parse_file(filename, use_cpp=True)
        return self.to_json(ast, **kwargs)

    def _parse_coord(self,coord_str):
        """ Parse coord string (file:line[:column]) into Coord object. """
        if coord_str is None:
            return None

        vals = coord_str.split(':')
        vals.extend([None] * 3)
        filename, line, column = vals[:3]
        return Coord(filename, line, column)

    def _convert_to_obj(self,value):
        """
        Convert an object in the dict representation into an object.
        Note: Mutually recursive with from_dict.

        """
        value_type = type(value)
        if value_type == dict:
            return self.from_dict(value)
        elif value_type == list:
            return [self._convert_to_obj(item) for item in value]
        else:
            # String
            return value

    def from_dict(self,node_dict):
        """ Recursively build an ast from dict representation """
        class_name = node_dict.pop('_nodetype')

        klass = getattr(c_ast, class_name)

        # Create a new dict containing the key-value pairs which we can pass
        # to node constructors.
        objs = {}
        for key, value in node_dict.items():
            if key == 'coord':
                objs[key] = self._parse_coord(value)
            else:
                objs[key] = self._convert_to_obj(value)

        # Use keyword parameters, which works thanks to beautifully consistent
        # ast Node initializers.
        return klass(**objs)

    def from_json(self,ast_json):
        """ Build an ast from json string representation """
        return self.from_dict(json.loads(ast_json))

