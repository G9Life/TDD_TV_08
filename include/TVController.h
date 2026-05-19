/**
 * Copyright 2020 by Samsung Electronics, Inc.,
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung.
 */

#ifndef TV_CONTROLLER_H
#define TV_CONTROLLER_H

#include "Tuner.h"
#include "remoteKey.h"
#include <string>
#include <vector>

class TVController {
private:
    static constexpr int MIN_CHANNEL = 0;
    static constexpr int MAX_CHANNEL = 99;

    Tuner* tuner;
    std::string processingCH;
    std::vector<int> favoriteChannels;
    std::vector<int> searchedChannels;

    bool isDigitKey(remoteKey key) const;
    char digitChar(remoteKey key) const;
    int getCurrentChannel() const;
    void validateChannel(int ch) const;
    int parseChannel(const std::string& digits) const;
    void setChannel(int ch);
    void setTunerCh();
    void confirmPendingDigits();
    void clearPendingDigits();
    void handleDigit(remoteKey key);
    void handleChannelUp();
    void handleChannelDown();
    void toggleFavorite();
    void handleNextFavorite();
    void handleChannelSearch();

public:
    explicit TVController(Tuner* tuner);
    void pushButton(remoteKey key);
};

#endif // TV_CONTROLLER_H
