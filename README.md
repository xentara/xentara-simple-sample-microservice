# Simple Sample Driver for Xentara
This project contains a simple sample microservice for Xentara. It requires the Xentara development environment, as well as a Xentara licence. You can get a Xentara licence in the [Xentara Online Shop](https://www.xentara.io/product/xentara-for-industrial-automation/).

The documentation for Xentara can be found at https://docs.xentara.io/xentara

This driver used the Xentara Utility Library, as well as the Xentara Plugin Framework. Docs can be found here:

- https://docs.xentara.io/xentara-utils/
- https://docs.xentara.io/xentara-plugin/

## Functionality
This microservice reads two inputs, and write the bigger one back to an output. It also sets a “safe” state when
it is not running correctly.

## Xentara Elements
This microservice supplies a [skill element](https://docs.xentara.io/xentara/xentara_skills.html#xentara_skill_elements) with model file descriptor
`@Skill.SimpleSampleMicroservice.Instance`.

The class can be found in the following files:

- [src/Instance.hpp](src/Instance.hpp)
- [src/Instance.cpp](src/Instance.cpp)

The *Instance* class publishes the following [attribute](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes):

- `executionState` contains *true* or *false*, depending on whether the microservice is currently running correctly or not.
- `executionTime` contains the last time the microservice was executed.
- `error` contains the error message if `executionState` is *false*.

The *Instance* class published the following [events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events):

- `executed` is raised whenever the microservice was executed correctly
- `executionError` is raised whenever an error occurs executing the microservice. This event is also raised when the microservice is suspended.

The *Instance* class published the following [tasks](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks):

- `execute` executes the microservice.

## The Sample Model
This project contains a sample model file [config/model.json](config/model.json). The sample model file generates two inputs
using a [signal generator](https://docs.xentara.io/xentara/xentara_signal_generator.html), and uses two
[registers](https://docs.xentara.io/xentara/xentara_signal_flow.html#xentara_signal_flow_registers) as outputs.
