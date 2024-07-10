#pragma once

#include "player.h"
#include "car.h"
#include "douro.h"
#include "camera.h"
#include "timer.h"
#include "effect.h" // �G�t�F�N�g�N���X���C���N���[�h
#include <vector>

// �Q�[���̉�ʏ��
#define GAME_STATE_TITLE    0
#define GAME_STATE_SELECT   1
#define GAME_STATE_PLAY     2
#define GAME_STATE_RESULT   3

// �Q�[���̓�Փx
#define GAME_MODE_EASY      0
#define GAME_MODE_NORMAL    1
#define GAME_MODE_HARD      2

class game_manager {
private:
    int game_state;     // �Q�[���̉�ʏ��
    int game_mode;      // �Q�[���̓�Փx
    int game_character; // �Q�[���Ŏg�p����L�����N�^�[

    int game_span;      // �Q�[���I����̑ҋ@����
    bool game_clear;    // �Q�[���N���A�t���O
    bool game_miss;     // �Q�[�����s�t���O

    player obj_player;          // �v���C���[�C���X�^���X
    std::vector<car> obj_car;   // �ԃC���X�^���X
    douro obj_douro;            // ���H�C���X�^���X
    camera obj_camera;          // �J�����C���X�^���X
    timer obj_timer;            // �^�C�}�[

    int car_span;       // �ԍ쐬�̌��Ԋu
    int car_interval;   // �ԍ쐬�̐����Ԋu
    int key_span;       // �L�[���͂̐����Ԋu

    int font_count_down;    // �J�E���g�_�E���p�̃t�H���g

    int bgm_handle;         // BGM�̃n���h��
    bool is_bgm_playing;    // BGM���Đ������ǂ����������t���O

    int se_handle;          // SE�̃n���h��
    bool is_se_playing;     // SE���Đ������ǂ����������t���O

    Effect effect;          // �G�t�F�N�g�C���X�^���X

public:
    game_manager();     // �R���X�g���N�^
    void update();      // �X�V
    void draw();        // �`��
    void finalize();    // ���f���f�[�^�̊J��
    void car_make();    // �Ԃ��쐬������폜�����肷����
};
