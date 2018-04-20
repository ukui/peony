Contributing to Peony Admin
==============================

The extension is licensed under the GPLv3, so you are free to use it, modify it
and improve it.
Here are some ways to contribute:


## Translating

The extension needs to be translated to more languages. You are welcome to
translate it to yours!

The translation template, in gettext format, is located in the `po` folder.
The `.po` files should be listed in the `gettext_create_translations()` command
of the *CMakeLists.txt*.

There are also some translatable strings in the XML file inside of the `polkit`
folder.

You can send the translation through a pull request or through
the [issue tracker][issues].


## Coding

You are welcome to fix bugs or add new features.
Find bugs that need to be fixed in the [issue tracker][issues].

The Python code was indented using tabs.

If you add any translatable string to the extension, please run the
"generate-pot-file.sh" script to update the translation template.



[issues]: https://github.com/infirit/peony-admin/issues
