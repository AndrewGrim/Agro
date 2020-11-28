# Code Style

## Constants and C-style Enums
Should follow `SCREAMING_CASE`.

## Variables
All varialbes should follow the `snake_case` convention.
Variables not meant to be used externally can be prefaced with an underscore
to denote either a temporary status or to prevent clashing.
Member variables that should not be used directly can be `m_snake_case`
to prevent clashing with function used for manipulation a member variable.

## Functions
All functions should follow the `pascalCase` convention.

## Structs, Classes, Enums, Unions
All of the above should follow the `CamelCase` convention.

## Braces
All `{ }` should be used like `if (condition) {` with the first brace on the same line.
The exception would be if you're using braces purely to create a specific scope.
All if statements and all loops should **ALWAYS** have braces even if the body
contains only one statement.

All language keywords should be followed by a single space ` ` even if not stricly required.
Like in a loop `for () {` or an if `if () {` or ` void func() override {`.