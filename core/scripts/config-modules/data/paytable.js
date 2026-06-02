
const PAYTABLE_CONFIG = [
    { symbol: 'WS', pay: 15,   count: 3 },
    { symbol: 'WS', pay: 200,  count: 4 },
    { symbol: 'WS', pay: 800,  count: 5 },    
    { symbol: 'H1', pay: 15,   count: 3 },
    { symbol: 'H1', pay: 200,  count: 4 },
    { symbol: 'H1', pay: 800,  count: 5 },
    { symbol: 'H2', pay: 12,   count: 3 },
    { symbol: 'H2', pay: 100,  count: 4 },
    { symbol: 'H2', pay: 500,  count: 5 },
    { symbol: 'H3', pay: 10,   count: 3 },
    { symbol: 'H3', pay: 60,   count: 4 },
    { symbol: 'H3', pay: 300,  count: 5 },
    { symbol: 'H4', pay: 6,    count: 3 },
    { symbol: 'H4', pay: 50,   count: 4 },
    { symbol: 'H4', pay: 200,  count: 5 },
    { symbol: 'H5', pay: 6,    count: 3 },
    { symbol: 'H5', pay: 30,   count: 4 },
    { symbol: 'H5', pay: 120,  count: 5 },
    { symbol: 'L6', pay: 3,    count: 3 },
    { symbol: 'L6', pay: 20,   count: 4 },
    { symbol: 'L6', pay: 100,  count: 5 },
    { symbol: 'L7', pay: 3,    count: 3 },
    { symbol: 'L7', pay: 15,   count: 4 },
    { symbol: 'L7', pay: 80,   count: 5 },
    { symbol: 'L8', pay: 3,    count: 3 },
    { symbol: 'L8', pay: 12,   count: 4 },
    { symbol: 'L8', pay: 60,   count: 5 },
    { symbol: 'L9', pay: 3,    count: 3 },
    { symbol: 'L9', pay: 10,   count: 4 },
    { symbol: 'L9', pay: 50,   count: 5 },
    { symbol: 'L10',pay: 3,   count: 3 },
    { symbol: 'L10',pay: 10,  count: 4 },
    { symbol: 'L10',pay: 30,  count: 5 },
    { symbol: 'SC', pay: 0,   count: 1 },
    { symbol: 'SC', pay: 0,   count: 2 },
    { symbol: 'SC', pay: 0,   count: 3 },
    { symbol: 'SC', pay: 0,   count: 4 },
    { symbol: 'SC', pay: 0,   count: 5 },
]

const PayTable  = [
    /* sym count = 3     4     5  */
    /*  WS  */   [ 15,   200,   800, ],
    /*  H1  */   [ 15,   200,   800, ],
    /*  H2  */   [ 12,   100,   500, ],
    /*  H3  */   [ 10,   60,    300, ],
    /*  H4  */   [  6,   50,    200, ],
    /*  H5  */   [  6,   30,    120, ],
    /*  L6  */   [  3,   20,    100, ],
    /*  L7  */   [  3,   15,     80, ],
    /*  L8  */   [  3,   12,     60, ],
    /*  L9  */   [  3,   10,     50, ],
    /*  L10  */  [  3,   10,     30, ],
    /*  SC  */   [  0,   0,      0, ],
];


module.exports  = {
    PAYTABLE_CONFIG,
    PayTable
}
