# -*- coding: utf-8 -*-
from setuptools import setup

packages = \
['gimli']

package_data = \
{'': ['*']}

setup_kwargs = {
    'name': 'gimli',
    'version': '0.1.0',
    'description': 'Python bindings for the gimli reference implementation',
    'long_description': '',
    'author': 'Anton Kueltz',
    'author_email': 'None',
    'maintainer': 'None',
    'maintainer_email': 'None',
    'url': 'None',
    'packages': packages,
    'package_data': package_data,
    'python_requires': '>=3.8,<4.0',
}
from build import *
build(setup_kwargs)

setup(**setup_kwargs)
