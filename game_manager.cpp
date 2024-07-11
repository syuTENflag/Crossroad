#include "game_manager.h"
#include "collision.h"
#include "DxLib.h" // Include DxLib header for sound functions

game_manager::game_manager()
    : effect(100, 60) // 最大パーティクル数と寿命を指定してエフェクトを初期化
{
    game_state = GAME_STATE_TITLE;
    game_character = GAME_CHARA_OJO;

    game_span = 0;
    game_clear = false;
    game_miss = false;

    car_span = 0;
    car_interval = 60;
    key_span = 0;

    // 車一個挿入
    obj_car.push_back(car());

    // フォントデータ
    font_count_down = CreateFontToHandle(NULL, 100, 10, -1);

    // BGMをロード
    bgm_handle = LoadSoundMem("./resorces/Sound/RUN.mp3"); // Soundファイルのパスを指定
    ChangeVolumeSoundMem(128, bgm_handle);
    is_bgm_playing = false;  // 初期状態ではBGMを再生しない

    // SEをロード
    se_handle = LoadSoundMem("./resorces/Sound/jiko.mp3"); // Soundファイルのパスを指定
    ChangeVolumeSoundMem(128, se_handle);
    is_se_playing = false; // 初期状態ではSEを再生しない

}

void game_manager::update() {
    switch (game_state) {
    case GAME_STATE_TITLE:
        for (unsigned int i = 0; i < obj_car.size(); i++) {
            obj_car.at(i).update(); // 車の移動
        }
        car_make(); // 車の追加

        // 難易度の選択
        if (key_span++ > 10) {
            if (CheckHitKey(KEY_INPUT_UP) == 1) {
                key_span = 0;
                if (--game_mode < 0) { game_mode = GAME_MODE_HARD; }
            }
            if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
                key_span = 0;
                if (++game_mode > GAME_MODE_HARD) { game_mode = GAME_MODE_EASY; }
            }
        }

        // スペースキーが押されたら、キャラクターセレクトへ移行
        if (game_span++ >= 30 && CheckHitKey(KEY_INPUT_SPACE) == 1) {
            // 難易度による、車の数の設定
            switch (game_mode) {
            case GAME_MODE_EASY: car_interval = 30; break;
            case GAME_MODE_NORMAL: car_interval = 10; break;
            case GAME_MODE_HARD: car_interval = 2; break;
            }

            // 移行処理
            clsDx();
            key_span = 0;
            game_span = 0;
            game_state = GAME_STATE_SELECT;
        }
        break;
    case GAME_STATE_SELECT:
        for (unsigned int i = 0; i < obj_car.size(); i++) {
            obj_car.at(i).update(); // 車の移動
        }
        car_make(); // 車の追加

        // キャラクターの選択
        if (key_span++ > 10) {
            if (CheckHitKey(KEY_INPUT_UP) == 1) {
                key_span = 0;
                if (--game_character < 0) { game_character = GAME_CHARA_REIMU; }
            }
            if (CheckHitKey(KEY_INPUT_DOWN) == 1) {
                key_span = 0;
                if (++game_character > GAME_CHARA_REIMU) { game_character = GAME_CHARA_OJO; }
            }
        }
        // スペースキーが押されたら、ゲームへ移行
        if (game_span++ >= 30 && CheckHitKey(KEY_INPUT_SPACE) == 1) {
            // キャラクターの読み込み
            switch (game_character) {
            case GAME_CHARA_OJO: obj_player = player(GAME_CHARA_OJO, VGet(0.0f, 0.0f, 0.0f)); break;
            case GAME_CHARA_ASN: obj_player = player(GAME_CHARA_ASN, VGet(0.0f, 100.0f, 0.0f)); break;
            case GAME_CHARA_REIMU: obj_player = player(GAME_CHARA_REIMU, VGet(0.0f, 100.0f, 0.0f)); break;
            }

            // 移行処理
            key_span = 0;
            game_span = 0;
            game_state = GAME_STATE_PLAY;

            // BGMを再生開始
            if (!is_bgm_playing) {
                PlaySoundMem(bgm_handle, DX_PLAYTYPE_LOOP);
                is_bgm_playing = true;
            }
        }
        break;
    case GAME_STATE_PLAY:
        if (key_span++ > 180) {
            obj_player.update(); // プレイヤーの操作
            obj_timer.update(); // タイマーの更新
        }
        else {
            obj_timer.initialize(); // タイマーの初期化
        }
        for (unsigned int i = 0; i < obj_car.size(); i++) {
            obj_car.at(i).update(); // 車の移動
        }
        obj_camera.update(obj_player.get_position()); // カメラの更新

        // クリアしている場合
        if (game_clear) {
            // 当たったらリザルトへ移行
            game_state = GAME_STATE_RESULT;
        }
        // 失敗している場合
        else if (game_miss) {
            // プレイヤーぐるぐる
            obj_player.rotation();

            // しばらく待ってからリザルトへ移行
            if (game_span++ >= 180) {
                game_state = GAME_STATE_RESULT;
            }
        }
        // プレイ中の場合
        else {
            // プレイヤーとゴールのあたり判定
            if (collision_player_goalbox(&obj_player)) {
                game_clear = true;
            }
            // プレイヤーと車たちのあたり判定
            for (unsigned int i = 0; i < obj_car.size(); i++) {
                if (collision_player_car(&obj_player, &obj_car.at(i))) {
                    game_miss = true;
                    obj_player.control_off();
                    obj_camera.track_off();
                    // SEを再生
                    PlaySoundMem(se_handle, DX_PLAYTYPE_BACK);
                    is_se_playing = true;
                    // エフェクトを生成
                    effect.emit(obj_player.get_position());
                }
            }
        }

        // 車の追加
        car_make();

        break;
    case GAME_STATE_RESULT:
        // スペースキーが押されたら、各種初期化してタイトルへ移行
        if (CheckHitKey(KEY_INPUT_SPACE) == 1) {
            // 各インスタンス
            obj_player = player(GAME_CHARA_ASN, VGet(0.0f, 0.0f, 0.0f));
            obj_car.clear();
            obj_camera = camera();

            // ゲームクリアの判定変数
            game_span = 0;
            game_clear = false;
            game_miss = false;

            // BGMを停止
            StopSoundMem(bgm_handle);
            is_bgm_playing = false;

            game_state = GAME_STATE_TITLE;
        }
        break;
    }

}

void game_manager::draw() {
    switch (game_state) {
    case GAME_STATE_TITLE:
        DrawFormatString(200, 100, GetColor(0, 0, 0), "\nスペースキーを押してはじめる.");
        DrawFormatString(200, 200, GetColor(0, 0, 0), "かんたん");
        DrawFormatString(200, 220, GetColor(0, 0, 0), "ふつう");
        DrawFormatString(200, 240, GetColor(0, 0, 0), "むずかしい");
        DrawFormatString(160, 200 + game_mode * 20, GetColor(0, 0, 0), "->");
        break;
    case GAME_STATE_SELECT:
        DrawFormatString(200, 100, GetColor(0, 0, 0), "キャラクターセレクト");
        DrawFormatString(200, 200, GetColor(0, 0, 0), "お嬢様");
        DrawFormatString(200, 220, GetColor(0, 0, 0), "アスナ");
        DrawFormatString(200, 240, GetColor(0, 0, 0), "霊夢");
        DrawFormatString(160, 200 + game_character * 20, GetColor(0, 0, 0), "->");
        break;
    case GAME_STATE_PLAY:
        // カウントダウン
        if (key_span <= 60) { key_span++; }
        else if (key_span <= 120) { DrawStringToHandle(200, 100, "3", GetColor(255, 0, 0), font_count_down); key_span++; }
        else if (key_span <= 180) { DrawStringToHandle(200, 100, "2", GetColor(255, 0, 0), font_count_down); }
        else if (key_span <= 240) { DrawStringToHandle(200, 100, "1", GetColor(255, 0, 0), font_count_down); }

        // ゲームモード表示
        switch (game_mode) {
        case GAME_MODE_EASY: DrawFormatString(10, 10, GetColor(255, 255, 255), "MODE[ EASY ]"); break;
        case GAME_MODE_NORMAL: DrawFormatString(10, 10, GetColor(255, 255, 255), "MODE[ NORMAL ]"); break;
        case GAME_MODE_HARD: DrawFormatString(10, 10, GetColor(255, 255, 255), "MODE[ HARD ]"); break;
        }

        // タイマー表示
        obj_timer.draw();

        break;
    case GAME_STATE_RESULT:
        if (game_clear) {
            switch (game_mode) {
            case GAME_MODE_EASY:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "ゴール");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            case GAME_MODE_NORMAL:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "ゴール！");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            case GAME_MODE_HARD:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "ゴール！！");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            }
        }
        else if (game_miss) {
            switch (game_mode) {
            case GAME_MODE_EASY:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "轢かれました。");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            case GAME_MODE_NORMAL:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "轢かれました。");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            case GAME_MODE_HARD:
                DrawFormatString(250, 100, GetColor(255, 100, 100), "轢かれました。");
                DrawFormatString(250, 150, GetColor(255, 100, 100), "\nスペースキーでタイトルに戻る.");
                break;
            }
        }
        break;
    }

    obj_player.draw(); // プレイヤークラス
    for (unsigned int i = 0; i < obj_car.size(); i++) {
        obj_car.at(i).draw(); // 車クラス
    }
    obj_douro.draw(); // 道路クラス
    // obj_camera.draw(); // カメラクラス
}

void game_manager::finalize() {
    // モデルハンドルの削除
    obj_player.finalize();
    obj_douro.finalize();
    for (unsigned int i = 0; i < obj_car.size(); i++) {
        obj_car.at(i).finalize();
    }

    // フォントデータの削除
    DeleteFontToHandle(font_count_down);

    // BGMの削除
    DeleteSoundMem(bgm_handle);
}

/*
//player player1 = player("./resorces/yukari/yukari.pmd", 14.0f, VGet(0.0f, 0.0f, 0.0f));
player player1 = player("./resorces/bokoboko.pmd", 20.0f, VGet(200.0f, 0.0f, 0.0f));
//player player3 = player("./resorces/79吹雪型3.1/79磯波v3.1.pmx", 10.0f, VGet(-200.0f, 0.0f, 0.0f));
*/

void game_manager::car_make() {
    const float road_far_left = -2500.0f; // 奥側の道路の左車線
    const float road_far_right = -1900.0f; // 奥側の道路の右車線
    const float road_this_left = -100.0f; // 手前側の道路の左車線
    const float road_this_right = -700.0f; // 手前側の道路の右車線

    int rand_num = 4; // 乱数の数

    // 難易度による変更を設定する
    switch (game_mode) {
    case GAME_MODE_EASY: rand_num = 3; break;
        case GAME_MODE_NORMAL: rand_num = 5; break;
        case GAME_MODE_HARD: rand_num = 5; break;
    }

    // 指定した間隔が経過したら車を追加する
    if (car_span++ >= car_interval) {
        switch (GetRand(rand_num)) {
        case 0: // 奥側の左
            obj_car.push_back(car(CAR_TYPE_LCAR, VGet(ROAD_LIMIT_LEFT, 100.0f, road_far_left), -50.0f));
            break;
        case 1: // 奥側の右
            obj_car.push_back(car(CAR_TYPE_LCAR, VGet(ROAD_LIMIT_LEFT, 100.0f, road_far_right), -50.0f));
            break;
        case 2: // 手前側の右
            obj_car.push_back(car(CAR_TYPE_RCAR, VGet(ROAD_LIMIT_RIGHT, 100.0f, road_this_right), 50.0f));
            break;
        case 3: // 手前側の左
            obj_car.push_back(car(CAR_TYPE_RCAR, VGet(ROAD_LIMIT_RIGHT, 100.0f, road_this_left), 50.0f));
            break;
        case 4: // 奥側
            if (GetRand(50) > 25) {
                obj_car.push_back(car(CAR_TYPE_RORA, VGet(ROAD_LIMIT_LEFT, 200.0f, (road_far_left + road_far_right) / 2.0f), -20.0f));
            }
            break;
        case 5: // 手前側
            if (GetRand(50) > 25) {
                obj_car.push_back(car(CAR_TYPE_RORA, VGet(ROAD_LIMIT_RIGHT, 200.0f, (road_this_right + road_this_left) / 2.0f), 20.0f));
            }
            break;
        }

        // 間隔を元に戻す
        car_span = 0;
    }

    // 無効な車を削除する
    for (auto itr = obj_car.begin(); itr != obj_car.end();) {
        if (itr->flag) { itr++; }
        else {
            itr->finalize();
            itr = obj_car.erase(itr);
        }
    }
}
