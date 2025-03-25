### The restricted_dictionary: dictionary with dependency management

While a regular dictionary simply stores key-value pairs, the restricted_dictionary adds the capability to define and enforce rules or constraints on the data it holds. This is achieved by allowing the user to set up dependencies between different key-value pairs, where the ability to set a particular key-value pair might be restricted based on the presence or value of another key-value pair.

The main.c's output is as follows:

has_restriction: Attempt to set 'ERPS=ON' is denied due to restriction 'QoS=ON'
Found 3 restrictions for 'ERPS=ON':
- SNTP=ON
- VLAN=ON
- QoS=ON

dumping:
                 QoS    [ON]
                ERPS    [OFF]
                SNTP    [OFF]
                VLAN    [OFF]
