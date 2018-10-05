# Coding Style #

## Indentation ##

- Spaces only, 4 spaces
- Namespace contents not indented
- Cases in `switch` constructs not indented
- Wrapped lines are indented with 8 spaces
- Line length hopefully 80 characters

## Braces ##

- Always use braces, also for single-line blocks!
- Opening brace on same line as e.g. if statement

  ```cpp
  if (foo) {
      doThis();
  }
  ```

## Name conventions ##

- Camel-Case for everything except constants
- functions, methods and members with `smallFirstLetter`
- Class names with `CapitalLetter`
- `THIS_IS_A_CONST`
- Meaningful variable names

## New-lines ##

- One new-line to separate methods and functions etc.
- Each namespace declaration in a single line
- New-line at end of file

## Includes ##

- Include order:
  1. Own file header (cpp file, relative include paths inside the project)
  2. stdlib and c-lib headers
  3. External library headers
  3. Internal header
- Each separated with one new-line (also for different external libraries)
- Relative includes for own headers

## Code Constructs ##

- Use initializer lists as much as possible

## Documentation ##

- Doxygen with javadoc-like auto-brief option (first sentence is brief description) in this format:

  ```cpp
  /**
   * A templatized global registry.
   *
   * To create a registree, that will be registered on application startup before
   * main use the provided macros. This will work on every supported compiler.
   *
   * This class is reentrant.
   *
   * @author jwienke
   * @tparam R type to register called registree.
   */
  template<class R>
  class Registry: public boost::noncopyable {
  public:

      /**
       * Registers a new registree in the Registry. This class takes over the
       * ownership of the pointer and manages deleting the pointer when needed.
       *
       * @param registree new registree to register with a very long description
       *                  that is wrapped to align with the first word after the
       *                  parameter name.
       * @param bar yet another long text that at least has one long line to wrape
       *            to display the indentation scheme in comments.
       * @throw std::invalid_argument if a registree with the same key is already
       *                              registered. In this case @c r is not owned
       *                              by this registry
       */
      void addRegistree(R *registree, const Foo &bar)

  }
  ```

- Documentation in header files
