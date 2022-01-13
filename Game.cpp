//
// Created by Maliq Tressa on 12/28/21.
//

#include "Game.h"

Game::Game() : firstMove(true)
{
}

void Game::promptGameMode()
{
	std::cout << "Enter '1' for 3x3 and input '2' for 5x5" << std::endl;
	int l_gameMode;
	std::cin >> l_gameMode;
	if (l_gameMode < 1 || l_gameMode > 2)
		throw std::exception();
	switch (l_gameMode)
	{
		case 1:
			width = 3;
			height = 3;
			_field.resize(3 * 3, ' ');
			break;
		case 2:
			width = 5;
			height = 5;
			_field.resize(5 * 5, ' ');
			break;
		default:
			throw std::exception();
	}
}

void Game::promptFirstMove()
{
	std::cout << "Do you want to move first?('y' for yes , 'n' for no)"
			  << std::endl;
	char response;
	std::cin >> response;
	if (response != 'y' && response != 'n')
		throw std::exception();
	if (response == 'y')
		firstMove = true;
	else
		firstMove = false;
}

void Game::promptGameType()
{
	std::cout << "Enter game mode:\n";
	std::cout << "\t'1' - Human VS AI\n";
	std::cout << "\t'2' - Human VS Human\n";
	std::cout << "\t'3' - AI VS AI" << std::endl;
	int l_gameType;
	std::cin >> l_gameType;
	if (l_gameType < 1 || l_gameType > 3)
		throw std::exception();
	switch (l_gameType)
	{
		case 1:
			gameType = HU_AI;
			promptFirstMove();
			Player1Type = (firstMove ? Human : AI);
			Player2Type = (firstMove ? AI : Human);
			break;
		case 2:
			gameType = HU_HU;
			Player1Type = Player2Type = Human;
			break;
		case 3:
			gameType = AI_AI;
			Player1Type = Player2Type = AI;
			break;
		default:
			throw std::exception();
	}
}

void Game::promptInput()
{
	promptGameMode();
	promptGameType();
}

void Game::start()
{
    promptInput();
    fillBestIndexMap();
	while (true)
	{
		renderField();
		makePlayer1Move();
		renderField();
		if (isGameOver(_field))
			break;
		makePlayer2Move();
		if (isGameOver(_field))
			break;
	}
	printGameResult();
}

void Game::makePlayer1Move()
{
	if (Player1Type == AI)
	{
		int idx = computeBestMove(_field, 'X', true).first;
		_field[idx] = 'X';
	}
	else
	{
		auto cell = promptCell();
		_field[cell.second - 1 + (cell.first - 1) * width] = 'X';
	}
}

void Game::makePlayer2Move()
{
	if (Player2Type == AI)
	{
		int idx = computeBestMove(_field, 'O', true).first;
		_field[idx] = 'O';
	}
	else
	{
		auto cell = promptCell();
		_field[cell.second - 1 + (cell.first - 1) * width] = 'O';
	}
}

std::pair<int, int> Game::promptCell() const
{
	std::cout << "Enter [raw] and [column]: ";
	int r, c;
	std::cin >> r >> c;
	if ((r < 1 || r > height) || (c < 1 || c > width))
		throw std::exception();
	return (std::make_pair(r, c));
}

void Game::printGameResult() const
{
    if (isDraw(_field))
    {
        std::cout << "Draw!" << std::endl;
        return;
    }
    std::string winner;
    switch (gameType) {
        case HU_AI:
            winner = (!(isPlayerWin(_field, 'X') != !(firstMove))) ? ("AI") : ("Human");
            break;
        case HU_HU:
            winner = "Player" + ((isPlayerWin(_field, 'X')) ? (static_cast<std::string>("1")) : (static_cast<std::string>("2")));
            break;
        case AI_AI:
            winner = "AI " + ((isPlayerWin(_field, 'X') ? (static_cast<std::string>("1")) : (static_cast<std::string>("2"))));
            break;
    }
    std::cout << winner << " is WINNER!" << std::endl;
}

std::pair<int, int>
Game::computeBestMove(const std::vector<char> &l_field, char sign, bool useAsync)
{
    int winIndex = getInstantWinIndex(l_field, getEnemySign(sign));
    if (winIndex != -1)
    {
        if ((sign != 'O') != (!firstMove))
            return (std::make_pair(winIndex, 0));
        else
            return (std::make_pair(winIndex, 100));
    }
    winIndex = getInstantWinIndex(l_field, sign);
    if (winIndex != -1)
    {
        if ((sign != 'O') != (!firstMove))
            return (std::make_pair(winIndex, 100));
        else
            return (std::make_pair(winIndex, 0));
    }
	static auto comp = [](const auto &lhs, const auto &rhs)
	{
		return lhs.second < rhs.second;
	};

	typedef std::vector<std::pair<int, int>> pair_vec;

	std::vector<int>	freeIndexes = getFreeIndexes(l_field);
    int                 freeIndexCount = freeIndexes.size();

    if (freeIndexCount > 9)
    {
        int temp = getBestIndexFromMap(l_field);
        if (temp != -1)
        {
            if ((sign != 'O') != (!firstMove))
                return (std::make_pair(temp, 100));
            else
                return (std::make_pair(temp, 0));
        }
    }

	pair_vec			results;
    std::vector<std::vector<char>> fieldCopies(freeIndexes.size(), l_field);
    std::vector<std::pair<int, std::future<std::pair<int, int>>>> asyncResults;
    if (useAsync)
        asyncResults.reserve(freeIndexes.size());

	for (int i = 0; i < freeIndexes.size(); ++i) {
        std::vector<char> &copy = fieldCopies[i];
        int idx = freeIndexes[i];

        copy[idx] = sign;
    }
    for (int i = 0; i < freeIndexes.size(); ++i) {
        std::vector<char>   &copy = fieldCopies[i];
        int                 idx = freeIndexes[i];

		if (isGameOver(copy))
		{
			if (!isPlayerWin(copy, 'X') != !firstMove)
            {
//                return (std::make_pair(idx, 0));
                results.emplace_back(idx, 0);
            }
			else
            {
                return (std::make_pair(idx, 100));
//                results.emplace_back(idx, 100);
            }
		}
        else
        {
            if (useAsync)
            {
                auto temp = std::async(std::launch::async,
                                       &Game::computeBestMove,
                                       *this,
                                       std::ref(fieldCopies[i]),
                                       sign,
                                       false);
                asyncResults.emplace_back(idx, std::move(temp));
            }
            else
            {
                results.emplace_back(idx,
                                     computeBestMove(copy, getEnemySign(sign)).second);
            }
        }
	}
    if (useAsync)
    {
        for (auto& x : asyncResults)
        {
            results.emplace_back(x.first, x.second.get().second);
        }
    }
	pair_vec::iterator bestValueIter;
	if ((sign != 'O') != (!firstMove)) // 'X' XOR firstMove
		bestValueIter = std::max_element(results.begin(), results.end(),
										 comp);
	else
		bestValueIter = std::min_element(results.begin(), results.end(),
										 comp);
	std::pair<int, int> ret = std::make_pair(bestValueIter->first,
											 std::accumulate(results.begin(),
															 results.end(),
															 0,
															 [](const auto &a,
																const auto &b)
															 {
																 return (a +
																		 b.second);
															 }) /
											 results.size());
//    for (int i = 0; i < level; ++i) fs << '\t';
//    for (auto& x : results) fs << x.first << ' ' << x.second << " | ";
//    fs << '\n';
//    fs.close();
	return (ret);
}

std::vector<int> Game::getFreeIndexes(const std::vector<char>& l_field)
{
	std::vector<int> freeIdxs;
	for (int i = 0; i < l_field.size(); ++i)
	{
		if (l_field[i] == ' ')
			freeIdxs.push_back(i);
	}
	return (freeIdxs);
}

char Game::getEnemySign(char mySign)
{
	if (mySign == 'X')
		return ('O');
	else if (mySign == 'O')
		return ('X');
	else
		throw std::exception();
}

bool Game::isGameOver(const std::vector<char>& l_field) const
{
	if (getFreeIndexes(l_field).empty() || isPlayerWin(l_field, 'X') || isPlayerWin(l_field, 'O') || isDraw(l_field))
		return (true);
	return (false);
}

bool Game::isFieldEmpty() const
{
	for (char c: _field)
	{
		if (c != ' ')
			return (false);
	}
	return (true);
}

bool Game::isPlayerWin(const std::vector<char>& l_field, char sign) const
{
	int verticalCounter = 0;
	int horizontalCounter = 0;
	int diagonalCounter = 0;
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (l_field[j + width * i] == sign)
				++horizontalCounter;
			if (l_field[i + width * j] == sign)
				++verticalCounter;
		}
		if (horizontalCounter == width)
			return (true);
		if (verticalCounter == width)
			return (true);
		horizontalCounter = 0;
		verticalCounter = 0;
	}
	for (int i = 0; i < width; ++i)
	{
		if (l_field[i + width * i] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == width)
		return (true);
	diagonalCounter = 0;
	for (int i = 0; i < width; ++i)
	{
		if (l_field[i + width * (width - 1 - i)] == sign)
			++diagonalCounter;
	}
	if (diagonalCounter == width)
		return (true);
	return (false);
}

char Game::getFieldSign(int x, int y)
{
	return (_field[x + width * y]);
}

bool Game::isDraw(const std::vector<char>& l_field) const
{
	if (getFreeIndexes(l_field).empty() && !isPlayerWin(l_field, 'X') && !isPlayerWin(l_field, 'O'))
		return (true);
	return (false);
}

void Game::renderField() const
{
	for (int i = 0; i < 4 * width + 1; ++i) std::cout<<'_';
	std::cout<<'\n';

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < width; j++)
		{
			std::cout<<"| "<<_field[j + i * width]<<' ';
		}
		std::cout<<"|\n";
	}

	for (int i = 0; i < width; ++i)
	{
		std::cout<<"|---";
	}
	std::cout<<"|"<<std::endl;
}

void Game::renderField(const std::vector<char> &l_field) const
{
//    std::ofstream fs("log", std::ios::app);
//    for (int i = 0; i < 4 * width + 1; ++i) fs << '_';
//        fs << '\n';

//    for (int i = 0; i < width; ++i)
//    {
//        for (int j = 0; j < width; j++)
//        {
//            fs<<"| "<<l_field[j + i * width]<<' ';
//        }
//        fs<<"|\n";
//    }

//    for (int i = 0; i < width; ++i)
//    {
//        fs<<"|---";
//    }
//    fs<<"|"<<std::endl;
//    fs.close();
}

void Game::fillBestIndexMap() {
    auto &map = _bestIndexMap;
    map.resize(width * width);
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            ++map[i + width * j];
            ++map[j +  width * i];
        }
    }
    for (int i = 0; i < width; ++i)
    {
        ++map[i + width * i];
        ++map[(width - i - 1) + width * i];
    }
}

int Game::getBestIndexFromMap(const std::vector<char> &l_field) {
    static auto colIsEmpty = [&](int col) -> bool{
        for (int i = 0; i < width; ++i)
        {
            if (l_field[col + width * i] == 'X' || l_field[col + width * i] == 'O')
                return (false);
        }
        return (true);
    };
    static auto rawIsEmpty = [&](int raw) -> bool{
        for (int i = 0; i < width; ++i)
        {
            if (l_field[i + width * raw] == 'X' || l_field[i + width * raw] == 'O')
                return (false);
        }
        return (true);
    };

    std::vector<int> copy(_bestIndexMap);
    for (int i = 0; i < copy.size(); ++i){
        if (!colIsEmpty(i % width))
            ++copy[i];
        if (!rawIsEmpty(i / width))
            ++copy[i];
    }
    std::vector<std::pair<int, int>> sortedMap;
    int i = 0;
    std::transform(copy.begin(), copy.end(), std::back_inserter(sortedMap), [&](const auto& data){
        return (std::move(std::make_pair(i++, data)));
    });
    std::sort(sortedMap.begin(), sortedMap.end(), [](const auto& lhs, const auto& rhs){
        return lhs.second > rhs.second;
    });
    for (auto &x : sortedMap)
    {
        if (l_field[x.first] == ' ')
            return (x.first);
    }
    return (-1);
}

int Game::getInstantWinIndex(const std::vector<char> &l_field, char sign) {
    int col = -1;
    int raw = -1;
    int ascDiag = -1;
    int descDiag = -1;

    for (int i = 0; i < width; ++i)
    {
        int colCount = 0, rawCount = 0;
        for (int j = 0; j < width; ++j)
        {
            if (l_field[j + width * i] == ' '){
                raw = j + width * i;
            }
            else if (l_field[j + width * i] == sign){
                ++rawCount;
            }
            if (l_field[i + width * j] == ' '){
                col = i + width * j;
            }
            else if (l_field[i + width * j] == sign){
                ++colCount;
            }
        }
        if (rawCount == width - 1 && raw != -1)
            return (raw);
        if (colCount == width - 1 && col != -1)
            return (col);
        raw = col = -1;
    }
    int ascDiagCount = 0, descDiagCount = 0;
    for (int i = 0; i < width; ++i)
    {
        if (l_field[i + width * i] == ' ')
            descDiag = i + width * i;
        else if (l_field[i + width * i] == sign)
            ++descDiagCount;
        if (l_field[i + width * (width - i - 1)] == ' ')
            ascDiag = i + width * (width - i - 1);
        else if (l_field[i + width * (width - i - 1)] == sign)
            ++ascDiagCount;
    }
    if (descDiagCount == width - 1 && descDiag != -1)
        return (descDiag);
    if (ascDiagCount == width - 1 && ascDiag != -1)
        return (ascDiag);
    return (-1);
}