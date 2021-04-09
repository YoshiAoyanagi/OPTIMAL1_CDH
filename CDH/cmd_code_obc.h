#define			CODE_Cmd_NOP                                            0x00			//
#define			CODE_Cmd_DEPLOY_BLOCK                                   0x01			//引数であたえたブロックのブロックコマンドを展開してタイムラインコマンドに格納する
#define			CODE_Cmd_CLEAR_BLOCK                                    0x02			//引数で与えたブロックのブロックコマンドをクリアする
#define			CODE_Cmd_SET_BLOCK_POSITION                             0x03			//ブロックコマンドの登録位置を指定する。
#define			CODE_Cmd_ACTIVATE_BLOCK                                 0x04			//ブロックコマンドを有効化する
#define			CODE_Cmd_FINISH_TRANSITION                              0x05			//モード遷移終了トリガコマンド
#define			CODE_Cmd_SET_TRANSITION_TABLE                           0x06			//TransitionTable書き換えコマンド(from, to , index)
#define			CODE_Cmd_EXECUTE_APP                                    0x07			//アプリケーション実行コマンド
#define			CODE_Cmd_TDSP_SET_TASK_LIST_ID                          0x08			//タスクリストBC設定コマンド
#define			CODE_Cmd_ROTATE_BLOCK                                   0x09			//BC回転コマンド
#define			CODE_Cmd_COMBINE_BLOCK                                  0x0A			//BC融合コマンド
#define			CODE_Cmd_AH_register_rule                               0x0B			//アノマリルール設定コマンド(id, group)
#define			CODE_Cmd_AH_activate_rule                               0x0C			//アノマリルール有効化コマンド
#define			CODE_Cmd_AH_inactivate_rule                             0x0D			//アノマリルール無効化コマンド
#define			CODE_Cmd_AH_clear_log                                   0x0E			//アノマリログクリアコマンド
#define			CODE_Cmd_AS_set_id                                      0x0F			//アプリID設定コマンド
#define			CODE_Cmd_REGISTER_APP                                   0x10			//アプリ登録コマンド
#define			CODE_Cmd_INITIALIZE_APP                                 0x11			//アプリ初期化コマンド
#define			CODE_Cmd_SET_MODE_LIST                                  0x12			//
#define			CODE_Cmd_START_TRANSITION                               0x13			//運用モードを変更する
#define			CODE_Cmd_TlmRequest                                     0x14			//指定TLM_IDのテレメトリをダウンリンクする
#define			CODE_Cmd_TLM_SET_MODE                                   0x15			//OBCテレメトリモード設定：開始
#define			CODE_Cmd_SET_TLM_INTERVAL                               0x16			//interval[s]
#define			CODE_Cmd_CLEAR_ALL_TIMELINE                             0x18			//MOBCコマンドクリア(タイムライン)
#define			CODE_Cmd_CLEAR_TIMELINE_AT                              0x19			//MOBCに登録されているタイムラインコマンドから1つ選んで削除する
#define			CODE_Cmd_SET_OBC_TIME                                   0x1A			//MOBC時刻タグを入力値に設定する
#define			CODE_Cmd_STRX_SET_BR                                    0x20			//param0: 0=4kbps, 1:32kbps, 2:64kbps (IF基板とSTRX本体同時に変更)
#define			CODE_Cmd_STRX_TX_ON                                     0x21			//param0: 0=OFF, 1=ON
#define			CODE_Cmd_MUX_UART_SET                                   0x25			//
#define			CODE_Cmd_SYNC_TIME                                      0x26			//地上局時刻と衛星(OBC, RTC)時刻を同期させる
#define			CODE_Cmd_SYNC_OBC_TIME                                  0x27			//地上局時刻とOBC時刻を同期させる
#define			CODE_Cmd_SYNC_RTC_TIME                                  0x28			//地上局時刻とRTC時刻を同期させる
#define			CODE_Cmd_SYNC_GPS_TIME                                  0x29			//GPS時刻と衛星時刻を同期させる
#define			CODE_Cmd_FRAM_SYNC_RTC_ENABLE                           0x2A			//OBCリセット時にRTCと時刻同期させる(ENA:1, DIS:0)
#define			CODE_Cmd_FRAM_TLC_CLEAR_ALL                             0x2B			//OBC-FRAMのコマンドクリア(タイムライン)
#define			CODE_Cmd_FRAM_TLC_CLEAR_AT_TIME                         0x2C			//OBC-FRAMに登録されているタイムラインコマンドから時刻指定で1つ選んで削除する
#define			CODE_Cmd_FRAM_TLC_CLEAR_AT_NUMBER                       0x2D			//OBC-FRAMに登録されているタイムラインコマンドから番号指定で1つ選んで削除する
#define			CODE_Cmd_SOFTWARE_RESET                                 0x31			//ソフト強制リセット
#define			CODE_Cmd_FRAM_INITIALIZE                                0x32			//FRAMの初期化, 打ち上げ前モードにする
#define			CODE_Cmd_FRAM_CMD_INITIALIZE                            0x33			//FRAM(TLC, BC)を初期化する(全消去)
#define			CODE_Cmd_FRAM_DEPLOY_BLOCK                              0x34			//引数であたえたブロックのブロックコマンドを展開してタイムラインコマンドに格納する
#define			CODE_Cmd_FRAM_REMOVE_BLOCK                              0x35			//引数で与えたブロックのブロックコマンドをクリアする
#define			CODE_Cmd_FRAM_SET_BLOCK_POSITION                        0x36			//ブロックコマンドの登録位置を指定する。
#define			CODE_Cmd_FRAM_ACTIVATE_BLOCK                            0x37			//ブロックコマンドの有効にする
#define			CODE_Cmd_FRAM_REMOVE_BLOCK_AT_POSITION                  0x38			//指定IDの位置のブロックコマンドを消去する
#define			CODE_Cmd_FRAM_REMOVE_ALL_BLOCK                          0x39			//登録済みのブロックコマンドの情報をすべて消去
#define			CODE_Cmd_FRAM_REQUEST_BC_INFO                           0x3A			//指定IDのブロックコマンド情報をダウンリンクする
#define			CODE_Cmd_FRAM_REQUEST_BC_PARAM                          0x3B			//指定ID, 位置のブロックコマンドの実行パケットをダウンリンクする
#define			CODE_Cmd_STRX_SET_TX_PWR                                0x40			//Power Modeの切り替え
#define			CODE_Cmd_STRX_OTC_ENABLE                                0x41			//0: DIS, 1:EN
#define			CODE_Cmd_STRX_OTC_SET_THRESHOLD                         0x42			//Temperature[degC]
#define			CODE_Cmd_SET_STX_ONOFF_DURATION                         0x44			//param1: on時間[s]，param2: off時間[s]
#define			CODE_Cmd_SET_STX_BURST_DURATION                         0x45			//連続送信時間を設定[s]
#define			CODE_Cmd_EPS_RESET                                      0x50			//0: MAIN, 1:SUB
#define			CODE_Cmd_EPS_UVC_ENABLE                                 0x51			//0: DIS, 1:EN
#define			CODE_Cmd_EPS_UVC_SET_BAT_V                              0x52			//
#define			CODE_Cmd_EPS_SELECT                                     0x53			//0:ChA, 1:ChB
#define			CODE_Cmd_SRX_RESET                                      0x55			//
#define			CODE_Cmd_ADCS_POWER                                     0x60			//
#define			CODE_Cmd_RW_ENABLE                                      0x61			//
#define			CODE_Cmd_HEATER_POWER                                   0x62			//
#define			CODE_Cmd_RW_POWER                                       0x63			//
#define			CODE_Cmd_MSN_unreg_ENABLE                               0x64			//
#define			CODE_Cmd_MSN_5V_POWER                                   0x65			//
#define			CODE_Cmd_SUBCAM_POWER                                   0x66			//
#define			CODE_Cmd_SF_POWER                                       0x67			//
#define			CODE_Cmd_MTQ_POWER_OBC                                  0x68			//
#define			CODE_Cmd_POWER_SAFE_OFF                                 0x69			//
#define			CODE_Cmd_UNREG_POWER                                    0x70			//
#define			CODE_Cmd_POWER_REBOOT                                   0x71			//
#define			CODE_Cmd_OBC_MTQ_X                                      0x72			//
#define			CODE_Cmd_OBC_MTQ_Y                                      0x73			//
#define			CODE_Cmd_OBC_MTQ_Z                                      0x74			//
#define			CODE_Cmd_AQU_5V_POWER                                   0x75			//
#define			CODE_Cmd_OVCO_5V_POWER                                  0x76			//
#define			CODE_Cmd_OBC_DR_RECORD                                  0x80			//
#define			CODE_Cmd_OBC_DR_REPLAY_PARAM_SET                        0x81			//
#define			CODE_Cmd_OBC_DR_REPLAY                                  0x82			//
#define			CODE_Cmd_OBC_DR_ERASE                                   0x83			//
#define			CODE_Cmd_MSN_DR_RECORD_ENABLE                           0x84			//
#define			CODE_Cmd_MSN_DR_REPLAY_PARAM_SET                        0x85			//
#define			CODE_Cmd_MSN_DR_REPLAY                                  0x86			//
#define			CODE_Cmd_MSN_DR_ERASE                                   0x87			//
#define			CODE_Cmd_OBC_DR_INITIALIZE                              0x88			//
#define			CODE_Cmd_MSN_DR_INITIALIZE                              0x89			//
#define			CODE_Cmd_OBC_DR_DIRECT_REPLAY                           0x8A			//
#define			CODE_Cmd_MSN_DR_DIRECT_REPLAY                           0x8B			//
#define			CODE_Cmd_STX_SET_OUTPORT                                0xA1			//
#define			CODE_Cmd_STX_SET_MOD                                    0xA2			//
#define			CODE_Cmd_STX_SET_PN_TEST                                0xA3			//
#define			CODE_Cmd_SET_STRX_LOCK_REPLY_TIME                       0xA4			//
#define			CODE_Cmd_TlmRequest_via_SF                              0xBB			//SF経由で指定IDのテレメトリをダウンリンクする
#define			CODE_Cmd_Set_Fwd_TlmID_via_SF                           0xBC			//SF経由で指定IDのテレメトリをダウンリンクする
#define			CODE_Cmd_AutoControl_set_restart_counter                0xC0			//再起動自動処理のカウンターを設定する
#define			CODE_Cmd_AutoControl_set_action                         0xC1			//再起動自動処理の内容を設定する
#define			CODE_Cmd_AutoControl_remove_action                      0xC2			//再起動自動処理の消去（ID指定)
#define			CODE_Cmd_AutoControl_remove_all_action                  0xC3			//再起動自動処理の全消去
#define			CODE_Cmd_AutoControl_execute                            0xC4			//再起動自動処理の実行
#define			CODE_Cmd_ClearRebootCount                               0xC5			//OBC再起動回数のクリア
#define			CODE_Cmd_SET_TLMD_ANOMALY_ENABLE                        0xC6			//TLMDアノマリの有効/無効
#define			CODE_Cmd_RETERN_CMD_OBC_UTIME                           0xC7			//OBC UTIMEをコマンドで返答する（返してほしいCMD_IDを指定する）
#define			CODE_Cmd_MSN_DR_ENABLE_OBC_TIMETAG                      0xC8			//ミッションテレメをMSN_DRに保存するときに～TLM_TIMEをOBC_UTIMEにセットする
