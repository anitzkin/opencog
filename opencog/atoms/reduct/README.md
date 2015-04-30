
Reduct-inspired term reduction for atoms
----------------------------------------

This is an experimental implementation of reduct-type functionality
in the atomspace. Its experimental because its not really designed for
performance nor for extensibility, nor anything else. Its more of a
proof-of-concept.  There are surely other ways of doing this, and some
of them might be better.

Anyway... try this at the guyile prompt:
```
(cog-reduce! (PlusLink (NumberNode 2) (NumberNode 2)))
```

you should see `(NumberNode 4)` as the output.

A more challenging example:
```
(cog-reduce! (PlusLink (NumberNode 0) (VariableNode "$x")))
```

should yeild `(VariableNode "$x")` -- that is, adding zero to something
has no effect!

Some more interesting examples:
```
(cog-reduce! (PlusLink (NumberNode 2) (VariableNode "$x") (NumberNode -2)))
```

should yeild `(VariableNode "$x")` -- the +2 and -2 cancel.

```
(cog-reduce! (PlusLink (NumberNode 2) (VariableNode "$x") (NumberNode 2)))
```

should yeild `(PlusLink (VariableNode "$x") (NumberNode 4))` --
the +2 and +2 sum.

Using the same ideas:
```
(cog-reduce! (TimesLink (NumberNode 1) (VariableNode "$x")))

(cog-reduce!
   (TimesLink
      (VariableNode "$y")
      (NumberNode 1)
      (VariableNode "$x")))

(cog-reduce!
   (TimesLink
      (VariableNode "$y")
         (NumberNode 0.5)
         (VariableNode "$x")
         (PlusLink (NumberNode 1) (NumberNode 1))))
```

Not yet implemented: something that can reduce x+2x to 3x and x+(-x) to 0.
