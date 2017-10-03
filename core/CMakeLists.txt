add_library(
        core
        result.h
        palette.h
        core_types.h
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
        timer_pool.cpp timer_pool.h
        state_stack.cpp state_stack.h
        views/label.cpp views/label.h
        views/caret.cpp views/caret.h
        font_family.cpp font_family.h
        views/panel.cpp views/panel.h
        symbol_table.cpp symbol_table.h
        views/button.cpp views/button.h
        views/textbox.cpp views/textbox.h
        palette_entry.cpp palette_entry.h
        command_parser.cpp command_parser.h
        views/pick_list.cpp views/pick_list.h
        views/dock_layout_panel.cpp views/dock_layout_panel.h)