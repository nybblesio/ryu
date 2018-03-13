add_library(
        core
        result.h
        palette.h
        rect.cpp rect.h
        view.cpp view.h
        result_message.h
        state.cpp state.h
        timer.cpp timer.h
        engine.cpp engine.h
        parser.cpp parser.h
        id_pool.cpp id_pool.h
        context.cpp context.h
        project.cpp project.h
        padding.cpp padding.h
        document.cpp document.h
        renderer.cpp renderer.h
        selection.cpp selection.h
        evaluator.cpp evaluator.h
        font_book.cpp font_book.h
        assembler.cpp assembler.h
        timer_pool.cpp timer_pool.h
        core_types.h core_types.cpp
        environment.cpp environment.h
        state_stack.cpp state_stack.h
        views/label.cpp views/label.h
        views/caret.cpp views/caret.h
        font_family.cpp font_family.h
        views/panel.cpp views/panel.h
        preferences.cpp preferences.h
        view_factory.cpp view_factory.h
        input_action.cpp input_action.h
        parser_types.cpp parser_types.h
        project_file.cpp project_file.h
        symbol_table.cpp symbol_table.h
        views/button.cpp views/button.h
        views/console.cpp views/console.h
        views/textbox.cpp views/textbox.h
        palette_entry.cpp palette_entry.h
        hex_formatter.cpp hex_formatter.h
        input_binding.cpp input_binding.h
        views/checkbox.cpp views/checkbox.h
        command_parser.cpp command_parser.h
        views/notebook.cpp views/notebook.h
        text_formatter.cpp text_formatter.h
        views/pick_list.cpp views/pick_list.h
        assembler_parser.cpp assembler_parser.h
        assembly_listing.cpp assembly_listing.h
        views/text_editor.cpp views/text_editor.h
        project_file_type.cpp project_file_type.h
        views/memory_editor.cpp views/memory_editor.h
        views/dock_layout_panel.cpp views/dock_layout_panel.h
        assembly_language_parser.cpp assembly_language_parser.h)