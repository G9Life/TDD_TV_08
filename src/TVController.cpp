/**
 * Copyright 2020 by Samsung Electronics, Inc.,
 *
 * This software is the confidential and proprietary information
 * of Samsung Electronics, Inc. ("Confidential Information").  You
 * shall not disclose such Confidential Information and shall use
 * it only in accordance with the terms of the license agreement
 * you entered into with Samsung.
 */

#include "TVController.h"
#include <algorithm>
#include <stdexcept>
#include <string>

TVController::TVController(Tuner* tuner)
    : tuner(tuner), processingCH("") {}

bool TVController::isDigitKey(remoteKey key) const {
    return key >= remoteKey::KEY_0 && key <= remoteKey::KEY_9;
}

bool TVController::isDigitOrConfirm(remoteKey key) const {
    return isDigitKey(key) || key == remoteKey::KEY_OK;
}

char TVController::digitChar(remoteKey key) const {
    return static_cast<char>('0' + (static_cast<int>(key) - static_cast<int>(remoteKey::KEY_0)));
}

int TVController::getCurrentChannel() const {
    return std::stoi(tuner->getCurrentCH());
}

void TVController::validateChannel(int ch) const {
    if (ch < MIN_CHANNEL || ch > MAX_CHANNEL) {
        throw std::invalid_argument("Invalid channel");
    }
}

int TVController::parseChannel(const std::string& digits) const {
    const int ch = std::stoi(digits);
    validateChannel(ch);
    return ch;
}

void TVController::setChannel(int ch) {
    validateChannel(ch);
    tuner->setCH(std::to_string(ch));
}

void TVController::confirmPendingDigits() {
    if (processingCH.empty()) {
        return;
    }
    const int ch = parseChannel(processingCH);
    processingCH.clear();
    setChannel(ch);
}

void TVController::setTunerCh() {
    confirmPendingDigits();
}

void TVController::clearPendingDigits() {
    processingCH.clear();
}

void TVController::handleDigit(remoteKey key) {
    processingCH += digitChar(key);
    if (processingCH.size() >= 2) {
        confirmPendingDigits();
    }
}

void TVController::handleChannelUp() {
    const int current = getCurrentChannel();
    if (searchedChannels.empty()) {
        const int next = (current == MAX_CHANNEL) ? MIN_CHANNEL : current + 1;
        setChannel(next);
        return;
    }

    const auto it = std::upper_bound(searchedChannels.begin(), searchedChannels.end(), current);
    if (it == searchedChannels.end()) {
        setChannel(searchedChannels.front());
    } else {
        setChannel(*it);
    }
}

void TVController::handleChannelDown() {
    const int current = getCurrentChannel();
    if (searchedChannels.empty()) {
        const int next = (current == MIN_CHANNEL) ? MAX_CHANNEL : current - 1;
        setChannel(next);
        return;
    }

    const auto it = std::lower_bound(searchedChannels.begin(), searchedChannels.end(), current);
    if (it == searchedChannels.begin()) {
        setChannel(searchedChannels.back());
    } else {
        setChannel(*std::prev(it));
    }
}

void TVController::toggleFavorite() {
    const int current = getCurrentChannel();
    const auto it = std::find(favoriteChannels.begin(), favoriteChannels.end(), current);
    if (it != favoriteChannels.end()) {
        favoriteChannels.erase(it);
    } else {
        favoriteChannels.push_back(current);
        std::sort(favoriteChannels.begin(), favoriteChannels.end());
    }
}

void TVController::handleNextFavorite() {
    if (favoriteChannels.empty()) {
        return;
    }

    const int current = getCurrentChannel();
    const auto it = std::upper_bound(favoriteChannels.begin(), favoriteChannels.end(), current);
    if (it == favoriteChannels.end()) {
        setChannel(favoriteChannels.front());
    } else {
        setChannel(*it);
    }
}

void TVController::handleChannelSearch() {
    searchedChannels.clear();
    const int startCh = getCurrentChannel();
    bool wrapped = false;

    for (int i = 0; i <= MAX_CHANNEL; ++i) {
        const std::string chStr = tuner->seekCH();
        if (chStr.empty()) {
            break;
        }

        const int ch = std::stoi(chStr);
        if (ch == startCh) {
            if (wrapped) {
                break;
            }
            wrapped = true;
        }

        if (std::find(searchedChannels.begin(), searchedChannels.end(), ch) == searchedChannels.end()) {
            searchedChannels.push_back(ch);
        }
    }

    std::sort(searchedChannels.begin(), searchedChannels.end());
}

void TVController::pushButton(remoteKey key) {
    if (isDigitKey(key)) {
        handleDigit(key);
        return;
    }

    if (key == remoteKey::KEY_OK) {
        setTunerCh();
        return;
    }

    if (!processingCH.empty()) {
        clearPendingDigits();
    }

    switch (key) {
        case remoteKey::KEY_CH_UP:
            handleChannelUp();
            break;
        case remoteKey::KEY_CH_DOWN:
            handleChannelDown();
            break;
        case remoteKey::KEY_CH_SEARCH:
            handleChannelSearch();
            break;
        case remoteKey::KEY_FAVORITE_ADD:
            toggleFavorite();
            break;
        case remoteKey::KEY_NEXT_FAVORITE:
            handleNextFavorite();
            break;
        default:
            break;
    }
}
